#ifndef SCHEDULEMODEL_H
#define SCHEDULEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDateTime>
#include <QVariant>
#include <QMap>
#include <QSortFilterProxyModel>

class ScheduleModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ScheduleModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        if(!parent.isValid())
            return m_times.size() + 1;
        else
            return 0;
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        if(!parent.isValid())
            return COL_COUNT;
        else
            return 0;
    }
    void setTimes(const QVector<QPair<QDateTime, QDateTime>> &times);

    enum Columns {COL_COME, COL_LEAVE, COL_DIFF, COL_RESULT, COL_COUNT};
    enum Roles {ROLE_DIFF = Qt::UserRole + 1, ROLE_RESULT, ROLE_DATETIME};
protected:
    void getHolidays();
private:
    QVector<QPair<QDateTime, QDateTime>> m_times;

    enum HolidayTypes { WORKING_DAY = 0, HOLIDAY = 2, SHORT_DAY = 3};
    QMap<QDate, int> m_holidays;
};

class ScheduleFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ScheduleFilterModel(QObject *parent): QSortFilterProxyModel(parent) {;}
    virtual ~ScheduleFilterModel() {;}

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // SCHEDULEMODEL_H
