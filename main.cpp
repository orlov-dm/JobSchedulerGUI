#include "ui/jobschedulermain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    JobSchedulerMain w;
    w.show();

    return a.exec();
}
