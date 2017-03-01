#ifndef SHEDULERCOMMON_H
#define SHEDULERCOMMON_H

#include <QDebug>
#define LOG qDebug() << this->metaObject()->className() << ": "

QString secondsToString(int value);

#endif // SHEDULERCOMMON_H
