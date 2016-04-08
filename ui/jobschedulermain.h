#ifndef JOBSCHEDULERMAIN_H
#define JOBSCHEDULERMAIN_H

#include <QMainWindow>
#include <QDate>

class Q_CORE_EXPORT QDate;
class ScheduleModel;

namespace Ui {
class JobSchedulerMain;
}

class JobSchedulerMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit JobSchedulerMain(QWidget *parent = 0);
    ~JobSchedulerMain();

protected:
    void setDate(const QDate &date);
    void changeMonth(bool forward);
    void loadMonth();

protected slots:
    void nextMonth();
    void previousMonth();

private:
    Ui::JobSchedulerMain *ui;

    QDate m_currentDate;
    ScheduleModel *m_scheduleModel = nullptr;

};

#endif // JOBSCHEDULERMAIN_H
