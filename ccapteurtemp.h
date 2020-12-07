#ifndef CCAPTEURTEMP_H
#define CCAPTEURTEMP_H

#include <fstream>
#include <iostream>
#include <istream>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BASE "/sys/bus/w1/devices/"    // chemin du fichier 1wire
#define PREFIXE "28-"       // début du lien recherché

class CCapteurTemp
{
public:
    CCapteurTemp();
    float getTemp();
};

#endif // CCAPTEURTEMP_H
