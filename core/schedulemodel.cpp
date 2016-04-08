#include "schedulemodel.h"

ScheduleModel::ScheduleModel()
{

}

QVariant ScheduleModel::data(const QModelIndex &index, int role) const
{
    QVariant res;

    if(index.row() != rowCount()-1)
    {

        const auto &dtCome = m_times[index.row()].first;
        const auto &dtLeave = m_times[index.row()].second;

        auto diffSecs = dtCome.secsTo(dtLeave);
        QTime diffTime = QTime(0,0).addSecs(diffSecs);
        QTime needToWork = QTime(8,30);
        QString resultString;
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
            res = QTime(0,0).secsTo(needToWork) - diffSecs;
        }
    }
    else
    {
        if(role == Qt::DisplayRole)
        {
            unsigned int sum = 0, temp = 0;
            int result = 0, tempResult = 0;
            bool ok = false;
            QTime diffTime = QTime(0,0);
            switch(index.column())
            {
                //Last row
                case COL_LEAVE:
                    res = tr("Sum:");
                    break;
                case COL_DIFF:
                    for(auto i = 0; i < rowCount() - 1; ++i)
                    {
                        temp = data(this->index(i, COL_DIFF), ROLE_DIFF).toUInt(&ok);
                        if(ok)
                            sum += temp;
                    }
                    diffTime = diffTime.addSecs(sum);
                    res = diffTime.toString();
                    break;
            case COL_RESULT:
                for(auto i = 0; i < rowCount() - 1; ++i)
                {
                    tempResult = data(this->index(i, COL_RESULT), ROLE_RESULT).toInt(&ok);
                    if(ok)
                        result += tempResult;
                }

                if(result > 0)
                {
                    diffTime = diffTime.addSecs(result);
                    res = "-"+diffTime.toString();
                }
                else
                {
                    diffTime = diffTime.addSecs(result*-1);
                    res = diffTime.toString();
                }

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
