#include "schedulemodel.h"
#include "schedulercommon.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

//-------------------------ScheduleModel----------------------------//
ScheduleModel::ScheduleModel()
{
    getHolidays();
}

QVariant ScheduleModel::data(const QModelIndex &index, int role) const
{
    QVariant res;

    if(index.row() != rowCount()-1)
    {
        auto dtCome = m_times[index.row()].first;
        auto dtLeave = m_times[index.row()].second;

        if(!(dtCome == dtLeave && dtCome.time() == QTime(0,0)))
        {
            //measurement error fix
            dtCome = dtCome.addSecs(-5*60);
            dtLeave = dtLeave.addSecs(5*60);
        }

        auto diffSecs = dtCome.secsTo(dtLeave);
        QTime diffTime = QTime(0,0).addSecs(diffSecs);
        QTime needToWork;
        auto holidayData = m_holidays.find(dtCome.date());
        if(holidayData != m_holidays.end())
        {
            if(holidayData.value() == HOLIDAY)
                needToWork = QTime(0,0);
            else if(holidayData.value() == SHORT_DAY)
                needToWork = QTime(7,30);
            else
                needToWork = QTime(8,30);
        }
        else
        {
            if(dtCome.date().dayOfWeek() == 6 || dtCome.date().dayOfWeek() == 7)
                needToWork = QTime(0,0);
            else
                needToWork = QTime(8,30);
        }

        QString resultString;
        if(index.column() == COL_RESULT)
        {
            //SKIP -> current day, no result yet
            if(dtCome.date() == QDateTime::currentDateTime().date())
                return res;
        }


        if(role == Qt::DisplayRole)
        {
            switch(index.column())
            {
            case COL_COME:
                if(dtCome.isValid())
                    res = dtCome.toString();
                break;
            case COL_LEAVE:
                if(dtLeave.isValid())
                    res = dtLeave.toString();
                break;
            case COL_DIFF:
                if(dtCome.isValid() && dtLeave.isValid())
                {
                    res = diffTime.toString();
                }
                break;
            case COL_RESULT:

                if(diffTime < needToWork)
                {
                    diffTime = QTime(0,0).addSecs(QTime(0,0).secsTo(needToWork) - QTime(0,0).secsTo(diffTime));
                    resultString += "-";
                }
                else if(diffTime > needToWork)
                {
                    diffTime = QTime(0,0).addSecs(QTime(0,0).secsTo(diffTime) - QTime(0,0).secsTo(needToWork));
                }
                else
                    diffTime = QTime(0,0);
                res = resultString + diffTime.toString();
            }
        }
        else
        if(index.column() == COL_DIFF && role == ROLE_DIFF)
        {            
            res = diffSecs;
        }
        else
        if(index.column() == COL_RESULT && role == ROLE_RESULT)
        {            
            auto resSecs = QTime(0,0).secsTo(needToWork) - diffSecs;
            res = resSecs;
        }
        else
        if((index.column() == COL_COME || index.column() == COL_LEAVE) && role == ROLE_DATETIME)
        {
            if(index.column() == COL_COME)
                res = dtCome.toTime_t();
            else
                res = dtLeave.toTime_t();
        }
    }
    else
    {
        if(role == Qt::DisplayRole)
        {
            unsigned int sum = 0, temp = 0;
            int result = 0, tempResult = 0;
            bool ok = false;
            switch(index.column())
            {
            //Last row
            case COL_LEAVE:
                res = tr("Sum:");
                break;
            case COL_DIFF:
            {
                for(auto i = 0; i < rowCount() - 1; ++i)
                {
                    temp = data(this->index(i, COL_DIFF), ROLE_DIFF).toUInt(&ok);
                    if(ok)
                        sum += temp;
                }                
                res = secondsToString(sum);
                break;
            }
            case COL_RESULT:
                for(auto i = 0; i < rowCount() - 1; ++i)
                {
                    tempResult = data(this->index(i, COL_RESULT), ROLE_RESULT).toInt(&ok);
                    if(ok)
                        result += tempResult;
                }
                //if result < 0 it's overwork
                result *= -1;

                res = secondsToString(result);
                break;

            }
        }
    }
//    else
//        res = QAbstractItemModel::data(index, role);
    return res;
}

QVariant ScheduleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res;
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case COL_COME:
            res = tr("Coming Time");
            break;
        case COL_LEAVE:
            res = tr("Leaving Time");
            break;
        case COL_DIFF:
            res = tr("Difference");
            break;
        case COL_RESULT:
            res = tr("Result");
            break;
        }
    }
    else
        res = QAbstractTableModel::headerData(section, orientation, role);
    return res;
}

void ScheduleModel::setTimes(const QVector<QPair<QDateTime, QDateTime> > &times)
{
    beginResetModel();    
    m_times = times;
    endResetModel();
}

void ScheduleModel::getHolidays()
{
    LOG << "Get Holidays";
    QFile f(":/json/holidays.json");
    if(f.open(QIODevice::ReadOnly))
    {
        LOG << "Opened file";
        while(!f.atEnd())
        {
            auto line = f.readLine();
            if(!line.isEmpty() && !line.startsWith("#"))
            {
                //LOG << line;
                auto object = QJsonDocument::fromJson(line).object();
                auto value = object.value("data");
                if(value != QJsonValue::Undefined)
                {
                    object = value.toObject();
                    auto year = QDateTime::currentDateTime().date().year();
                    value = object.value(QString::number(year));
                    if(value != QJsonValue::Undefined)
                    {
                        //found current year
                        object = value.toObject();
                        //LOG << object;
                        QJsonObject monthObject;
                        for(auto itMonth = object.begin(); itMonth != object.end(); ++itMonth)
                        {
                            //month
                            monthObject = itMonth.value().toObject();
                            QJsonObject dayObject;
                            for(auto itDay = monthObject.begin(); itDay != monthObject.end(); ++itDay)
                            {
                                //day
                                dayObject = itDay.value().toObject();
                                auto value = dayObject.value("isWorking");
                                if(value != QJsonValue::Undefined)
                                {
                                    auto date = QDate(year, itMonth.key().toInt(), itDay.key().toInt());
                                    m_holidays.insert(date, value.toInt());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}



//----------------------ScheduleFilterModel-------------------------//
bool ScheduleFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool filterAccept = true;
    LOG << sourceRow << " " << sourceParent;
    auto comeIndex = sourceModel()->index(sourceRow, ScheduleModel::COL_COME, sourceParent);
    auto leaveIndex = sourceModel()->index(sourceRow, ScheduleModel::COL_LEAVE, sourceParent);
    if(comeIndex.isValid() && leaveIndex.isValid())
    {
        auto comeTime = comeIndex.data(ScheduleModel::ROLE_DATETIME).toInt();
        auto comeDT = QDateTime::fromTime_t(comeTime);
        auto leaveTime = leaveIndex.data(ScheduleModel::ROLE_DATETIME).toInt();
        auto leaveDT = QDateTime::fromTime_t(leaveTime);
        if(comeDT.time() == leaveDT.time() && comeDT.time() == QTime(0, 0)
                || comeDT.date().dayOfWeek() == 6 || comeDT.date().dayOfWeek() == 7)
            filterAccept = false;
    }
    return filterAccept;
}
