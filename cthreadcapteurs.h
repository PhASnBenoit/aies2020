#ifndef CTHREADCAPTEURS_H
#define CTHREADCAPTEURS_H

#include <QObject>
#include <QWidget>
#include <QTimer>

#include "csharedmemory.h"
#include "ccapteurpres.h"
#include "ccapteurtemp.h"
#include "cadafruit_sgp30.h"

class CThreadCapteurs : public QObject
{
    Q_OBJECT
public:
    explicit CThreadCapteurs(QObject *parent = nullptr);
    ~CThreadCapteurs();

private:
    CSharedMemory *shm;
    QTimer *tmr;
    CCapteurPres *captPres;
    CCapteurTemp *captTemp;
    CAdafruit_SGP30 *captQa;
signals:

public slots:
    void on_timeout();
    void go();
};

#endif // CTHREADCAPTEURS_H
