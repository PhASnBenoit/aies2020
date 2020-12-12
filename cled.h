#ifndef CLED_H
#define CLED_H

#include <QObject>
#include "cgpio.h"

#define LED 27

class CLed : public QObject
{
    Q_OBJECT
public:
    explicit CLed(QObject *parent = nullptr);
    ~CLed();

private:
    CGpio *gpioLed;

signals:

public slots:
    int ledOn();
    int ledOff();
};

#endif // CLED_H
