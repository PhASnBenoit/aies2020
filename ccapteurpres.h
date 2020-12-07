#ifndef CCAPTEURPRES_H
#define CCAPTEURPRES_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include "cgpio.h"

class CCapteurPres : public QObject
{

    Q_OBJECT

public:
   explicit CCapteurPres(QObject *parent = NULL);
   ~CCapteurPres();
   bool getPresence();
private:
   CGpio *mPres;
   int varState;
signals:
   void sigAffPres(int);
};

#endif // CCAPTEURPRES_H
