#ifndef CBDD_H
#define CBDD_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDate>
#include <QTime>

#include "ccapteurpres.h"
#include "cconfig.h"
//#include "pa.h"

class CBdd
{
public:
    CBdd();
    ~CBdd();

    //accesseurs bdd
    QString getRefreshTime();
    int getUrgencyState();
    QString getNoZone(QString mac);
    QString getNomZone(int NoZone);
    QString getNomPa(QString mac);
    QString getzone();
    QString getModeFonc(QString mac);
    QString getHstart(QString mac);
    QString getHstop(QString mac);
    QString getCtrlTv(QString mac);
    QString getPermaUpdateVer();
    QString getPermaUpdateDate();
    QString getDiffUpdateVer();
    QString getDiffUpdateDate();
    QList<QList<QString> > getActiveSlides(QString mac);
    QString getActiveFlash();
    QList<QString>  getImagesVideos(QString mac);

    //QString getListePages(int zone);
    int getIdleTime(QString mac);

    //mutateur bdd
    void setCapteurs(QString temp, QByteArray sd, QString pres, QString mac);
    void setSliderUpdate(QDateTime dateActu);
    void writeBdd(QString query);
    void switchDiffToPerma();

private:
    // Bdd tools
    QSqlDatabase Aies_bdd;
    //QSqlQuery Aies_query;
    CConfig *conf;

    // info bdd
    QString query;
    QString databaseType;
    QString databaseHostname;
    QString databaseName;
    QString databaseUsername;
    QString databasePassword;
    QString RpiData;
    QString SERVEUR;
};

#endif // CBDD_H
