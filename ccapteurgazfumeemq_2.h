#ifndef CCAPTEURGAZFUMEEMQ_2_H
#define CCAPTEURGAZFUMEEMQ_2_H

#include <QObject>

#include "cgpio.h"

#define CF 23  // GPIO23 pin 16

class CCapteurGazFumeeMQ_2: public QObject
{
    Q_OBJECT
public:
    explicit CCapteurGazFumeeMQ_2(QObject *parent = nullptr);
    ~CCapteurGazFumeeMQ_2();
    bool getCapteur();

private:
    CGpio *gpioCF;

signals:

public slots:

};

#endif // CCAPTEURGAZFUMEEMQ_2_H
