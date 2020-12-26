#ifndef CUTV_H
#define CUTV_H

#include <QObject>
#include "cgpio.h"

#define UTV 10  // n° GPIO

class CUTv : public QObject
{
    Q_OBJECT
public:
    explicit CUTv(QObject *parent = nullptr);
    ~CUTv();
    bool getU();

private:
    CGpio *mUTv;
    bool mValU;

signals:

public slots:
};

#endif // CUTV_H
