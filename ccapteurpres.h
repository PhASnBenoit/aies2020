#ifndef CCAPTEURPRES_H
#define CCAPTEURPRES_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include "cgpio.h"

#define PRES 17

class CCapteurPres : public QObject
{
    Q_OBJECT
public:
   explicit CCapteurPres(QObject *parent = nullptr);
   ~CCapteurPres();
   bool getPresence();
private:
   CGpio *mPres;
   int varState;
signals:
   void sigAffPres(int);
};

#endif // CCAPTEURPRES_H
