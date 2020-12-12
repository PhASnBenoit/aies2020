#ifndef CECRAN_H
#define CECRAN_H

#include <QObject>
#include "ctelecommanders232samsung.h"
#include "cgpio.h"

#define UTV 10  // n° GPIO

class CEcran : public QObject
{
    Q_OBJECT
public:
    explicit CEcran(QObject *parent = nullptr, int no = 1);
    ~CEcran();
    bool getU();

signals:

public slots:
    int putOnCec();
    int putOffCec();
    int putOnRs();
    int putOffRs();

private:
    CTelecommandeRs232Samsung *mProt;
    int mNoEcran;
    CGpio *mUTv;
    bool mValU;
};

#endif // CECRAN_H
