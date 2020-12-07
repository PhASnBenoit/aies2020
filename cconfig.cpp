 #include "cconfig.h"

CConfig::CConfig()
{
    QSettings settings("/home/pi/aies/aies2020/aies.ini", QSettings::IniFormat);
    BddType = settings.value("Bdd/type", "QMYSQL").toString();
    BddHostname = settings.value("Serveur/ip", "192.168.4.25").toString();
    BddName = settings.value("Bdd/name", "aies2018").toString();
    BddUsername = settings.value("Bdd/username", "aies2016").toString();
    BddPassword = settings.value("Bdd/password", "aies2016").toString();
    NumVersion = settings.value("Rpi/version", "1.0").toString();
    htDocs = settings.value("Serveur/htdocs", "/2020").toString();  // by PhA 2019-01-31
}

QString CConfig::getBddType()
{
    return BddType;
}

QString CConfig::getBddHostname()
{
    return BddHostname;
}

QString CConfig::getBddName()
{
    return BddName;
}

QString CConfig::getBddUsername()
{
    return BddUsername;
}

QString CConfig::getBddPassword()
{
    return BddPassword;
}

QString CConfig::getNumVersion()
{
    return NumVersion;
}

QString CConfig::getHtDocs()   // by PhA 2019-01-31
{
    return htDocs;
}
