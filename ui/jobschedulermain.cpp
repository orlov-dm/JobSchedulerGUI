#include "jobschedulermain.h"
#include "ui_jobschedulermain.h"
#include "WindowsEventParser/windowseventparser.h"
#include "core/schedulemodel.h"
#include "core/schedulercommon.h"

#include <QDate>
#include <QDateTime>
#include <QList>
#include <QPair>

JobSchedulerMain::JobSchedulerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JobSchedulerMain)
{
    ui->setupUi(this);
    ui->calendarWidget->hide();

    WindowsEventParser::getInstance().setFlags(ParserFlag::USE_SECURITY_EVENTS | ParserFlag::USE_SYSTEM_EVENTS);
    m_scheduleModel = new ScheduleModel();
    ui->table->setModel(m_scheduleModel);

    setDate(QDate::currentDate());

    connect(ui->tbNextMonth, &QToolButton::clicked, this, &JobSchedulerMain::nextMonth);
    connect(ui->tbPrevMonth, &QToolButton::clicked, this, &JobSchedulerMain::previousMonth);
}

JobSchedulerMain::~JobSchedulerMain()
{
    delete ui;
}

void JobSchedulerMain::setDate(const QDate &date)
{
    ui->leMonthYear->setText(date.toString("MMMM yyyy"));    
    m_currentDate.setDate(date.year(), date.month(), 1);

    //Disable Next if it's current month
    if(m_currentDate.addMonths(1) > QDate::currentDate())
        ui->tbNextMonth->setEnabled(false);
    else if (!ui->tbNextMonth->isEnabled())
        ui->tbNextMonth->setEnabled(true);
    loadMonth();
}

void JobSchedulerMain::changeMonth(bool forward)
{
    setDate(m_currentDate.addMonths(forward?1:-1));    
}

void JobSchedulerMain::nextMonth()
{
    changeMonth(true);
}

void JobSchedulerMain::previousMonth()
{
    changeMonth(false);    
}

void JobSchedulerMain::loadMonth()
{
    ui->table->setUpdatesEnabled(false);
    QVector<QPair<QDateTime, QDateTime>> times;
    QDateTime dt;
    auto today = QDate::currentDate();
    auto dtFrom = QDateTime(QDate(m_currentDate), QTime(0,0));
    auto dtTo = dtFrom.addMonths(1);
    auto eventInfos = WindowsEventParser::getInstance().getLogTimesByDate(dtFrom.toTime_t(), dtTo.toTime_t());
    auto *logOns = &eventInfos.first;
    auto *logOffs = &eventInfos.second;
    time_t logOn, logOff;
    bool fillWithEmpty;
    for(int i = 1; i <= m_currentDate.daysInMonth(); ++i)
    {                
        fillWithEmpty = true;
        dt.setDate(QDate(m_currentDate.year(),m_currentDate.month(),i));
        dt.setTime(QTime(0,0));
        logOn = logOff = dt.toTime_t();
        if(dt.date() > today) {
            //Don't try to load future dates
            break;
        }
        auto currentDay = dt.toTime_t();
        if(logOns->size() || logOffs->size())
        {
            auto logOnIt = logOns->find(currentDay);
            if(logOnIt != logOns->end())
            {
                logOn = logOnIt->second;
            }
            auto logOffIt = logOffs->find(currentDay);
            if(logOffIt != logOffs->end())
            {
                logOff = logOffIt->second;
            }
            if(logOn || logOff)
            {
                times.append({QDateTime::fromTime_t(logOn),QDateTime::fromTime_t(logOff)});
                fillWithEmpty = false;
            }
        }

        if(fillWithEmpty)
        {
            if(!(dt.date().dayOfWeek() >= 1 && dt.date().dayOfWeek() < 6))
            {
                fillWithEmpty = false;
            }
        }

        if(fillWithEmpty)
        {
            times.append({dt, dt});
        }
    }
    m_scheduleModel->setTimes(times);

    ui->table->resizeColumnsToContents();
    ui->table->setUpdatesEnabled(true);
}

void JobSchedulerMain::on_tbFilter_clicked(bool checked)
{
    LOG<<"on_tbFilter_clicked";
    if(checked)
    {
        ScheduleFilterModel *filterModel = new ScheduleFilterModel(m_scheduleModel);
        filterModel->setSourceModel(m_scheduleModel);
        filterModel->setFilterRole(ScheduleModel::ROLE_DATETIME);
        filterModel->setFilterKeyColumn(-1);
        //filterModel->setFilter
        ui->table->setModel(filterModel);
    }
    else
    {
        auto model = dynamic_cast<ScheduleFilterModel*>(ui->table->model());
        if(model)
            delete model;

        ui->table->setModel(m_scheduleModel);
    }
}

void JobSchedulerMain::on_tbRefreshMonth_clicked()
{
    loadMonth();
}
