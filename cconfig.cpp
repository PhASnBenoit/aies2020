 #include "cconfig.h"

CConfig::CConfig()
{
    QSettings settings("aies.ini", QSettings::IniFormat);
    BddType = settings.value("Bdd/type", "QMYSQL").toString();
    BddHostname = settings.value("Serveur/ip", "192.168.1.15").toString();
    BddName = settings.value("Bdd/name", "aies2020").toString();
    BddUsername = settings.value("Bdd/username", "aies2020").toString();
    BddPassword = settings.value("Bdd/password", "aies2020").toString();
    NumVersion = settings.value("Rpi/version", "4.0").toString();
    htDocs = settings.value("Serveur/htdocs", "/").toString();  // by PhA 2019-01-31
    cacheDir = settings.value("Rpi/cache", "/opt/aies/cache").toString();  // by PhA 22021-01-03
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

QString CConfig::getCacheDir()
{
    return cacheDir;
}
