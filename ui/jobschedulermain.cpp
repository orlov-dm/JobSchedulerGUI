#include "jobschedulermain.h"
#include "ui_jobschedulermain.h"
#include "WindowsEventParser/windowseventparser.h"
#include "core/schedulemodel.h"

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

    WindowsEventParser::getInstance().setFlags(ParserFlag::USE_SECURITY_EVENTS);
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
    m_currentDate = date;

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
    for(int i = 1; i <= m_currentDate.daysInMonth(); ++i)
    {
        dt.setDate(QDate(2016,m_currentDate.month(),i));
        auto now = dt.toTime_t();
        auto logOn = WindowsEventParser::getInstance().getLogOnTimeByDate(now);
        auto logOff = WindowsEventParser::getInstance().getLogOffTimeByDate(now);
        if(logOn || logOff)
            times.append({QDateTime::fromTime_t(logOn),QDateTime::fromTime_t(logOff)});
    }
    m_scheduleModel->setTimes(times);

    ui->table->resizeColumnsToContents();
    ui->table->setUpdatesEnabled(true);
}


