#include "schedulercommon.h"

QString secondsToString(int value)
{
    QString result = "";
    bool isNegative = value < 0?true:false;
    if(isNegative)
        value *= -1;
    int hours = value/60/60;
    int minutes = (value - hours*60*60)/60;
    int seconds = value - hours*60*60 - minutes*60;
    result = QString("%1:%2:%3").arg(hours).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    if(isNegative)
        result.prepend("-");

    return result;
}
