#include "cled.h"

CLed::CLed(QObject *parent) :
    QObject(parent)
{
    gpioLed = new CGpio(LED);
}

CLed::~CLed ()
{
    delete gpioLed;
}

int CLed::ledOn()
{
//  qDebug() << "CLed::ledOn";
  gpioLed->ecrire(1);
  return 0;
}

int CLed::ledOff()
{
    gpioLed->ecrire(0);
    return 0;
}

