#ifndef CGPIO_H
#define CGPIO_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>				
#include <iostream>
#include <time.h>
#include <sys/select.h> 
#include <string.h>
#include <cstdlib>

#define IN  0
#define OUT 1

class CGpio : public QObject
{
    Q_OBJECT

public :
    explicit CGpio(int addr);
    ~CGpio();
    int lire();
    int ecrire(int value);

private :
    char mFilename[50];
    int init();
    int gpioExport();
    int gpioUnexport();
    int gpioDirection(int dir);
    int gpioRead();
    int gpioWrite(int value);
    int mAddr;

signals:
    void erreur(QString msg);
};

#endif // CGPIO_H
