#ifndef CIR_H
#define CIR_H

#include <QObject>
#include <QProcess>

class CIr : public QObject
{
    Q_OBJECT

public:
    explicit CIr(QObject *parent = 0);
    ~CIr();
    void irOnOff();

signals:

public slots:

};

#endif // CIR_H
