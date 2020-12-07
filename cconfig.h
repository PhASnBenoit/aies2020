#ifndef CCONFIG_H
#define CCONFIG_H

#include <QObject>
#include <QSettings>

class CConfig
{
public:
    CConfig();
    QString getBddType();
    QString getBddHostname();
    QString getBddName();
    QString getBddUsername();
    QString getBddPassword();
    QString getNumVersion();
    QString getHtDocs();  // by PhA  2019-01-31
private:
    QString BddType;
    QString BddHostname;
    QString BddName;
    QString BddUsername;
    QString BddPassword;
    QString NumVersion;
    QString htDocs;
};

#endif // CCONFIG_H
