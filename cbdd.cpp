#include "cbdd.h"

CBdd::CBdd()
{
    conf = new CConfig();
    SERVEUR = conf->getBddHostname();
    QString databaseType = conf->getBddType();
    QString databaseHostname = conf->getBddHostname();
    QString databaseName = conf->getBddName();
    QString databaseUsername = conf->getBddUsername();
    QString databasePassword = conf->getBddPassword();
qDebug() << "[CBdd::CBdd] : " << databaseType;
qDebug() << "[CBdd::CBdd] : " << databaseName;
qDebug() << "[CBdd::CBdd] : " << databaseHostname;
    Aies_bdd = QSqlDatabase::addDatabase(databaseType);
    Aies_bdd.setDatabaseName(databaseName);
    Aies_bdd.setHostName(databaseHostname);
    Aies_bdd.setUserName(databaseUsername);
    Aies_bdd.setPassword(databasePassword);
    if(!Aies_bdd.open())
    {
        qDebug("[CBdd::CBdd]: BDD non ouverte");
        exit(1);
    }
}

CBdd::~CBdd()
{
    delete conf;
    Aies_bdd.close();
}

QString CBdd::getNoZone(QString mac)
{
    QString NoZone = "NoZoneEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getNoZone]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT zone_id FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        NoZone = Aies_query.value(0).toString();
//        qDebug() << "NoZone: " << NoZone;
        return NoZone;
    }
}

QString CBdd::getNomZone(int NoZone)
{
    QString NomZone = "NomZoneEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getNomZone]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT zones FROM zones WHERE idzone='"+QString::number(NoZone)+"'");
        Aies_query.next();
        NomZone = Aies_query.value(0).toString();
        qDebug() << "[CBdd::getNomZone]: NomZone: " << NomZone;
        return NomZone;
    }
}

QString CBdd::getNomPa(QString mac)
{
    QString NomPa = "NomPaEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getNomPa]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT name FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        NomPa = Aies_query.value(0).toString();
        qDebug() << "NomPa: " << NomPa;
        return NomPa;
    }
}

QString CBdd::getModeFonc(QString mac)
{
    QString ModeFonc = "ModeFoncEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getMarque]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT ModeFonc FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        ModeFonc = Aies_query.value(0).toString();
        qDebug() << "ModeFonc: " << ModeFonc;
        return ModeFonc;
    }
}

QString CBdd::getCtrlTv(QString mac)
{
    QString CtrlTv = "CtrlTvEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getCtrlTv]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT CtrlTv FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        CtrlTv = Aies_query.value(0).toString();
        //qDebug() << "CtrlTv: " << CtrlTv;
        return CtrlTv;
    }
}

QString CBdd::getPermaUpdateVer()
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getUpdateVer]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QString permaUpdateVer;
        QSqlQuery Aies_query("SELECT version FROM rpi_update WHERE id=1");
        Aies_query.next();
        permaUpdateVer = Aies_query.value(0).toString();
        return permaUpdateVer;
    }
}

QString CBdd::getDiffUpdateVer()
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getUpdateVer]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QString diffUpdateVer;
        QSqlQuery Aies_query("SELECT version FROM rpi_update WHERE id=2");
        Aies_query.next();
        diffUpdateVer = Aies_query.value(0).toString();
        qDebug() << "diffUpdateVer du Serveur : " << diffUpdateVer;
        return diffUpdateVer;
    }
}

QString CBdd::getPermaUpdateDate()
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getUpdateDate]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QString permaUpdateDate;
        QSqlQuery Aies_query("SELECT date FROM rpi_update WHERE id=1");
        Aies_query.next();
        permaUpdateDate = Aies_query.value(0).toString();
        qDebug() << "permaUpdateDate: " << permaUpdateDate;
        return permaUpdateDate;
    }
}

QString CBdd::getDiffUpdateDate()
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getUpdateDate]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QString diffUpdateDate = "none";
        QSqlQuery Aies_query("SELECT date FROM rpi_update WHERE id=2");
        while(Aies_query.next())
        {
            diffUpdateDate = Aies_query.value(0).toString();
        }
        return diffUpdateDate;
    }
}

void CBdd::switchDiffToPerma()
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::switchDiffToPerma]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT version, date, date_create, path, size FROM rpi_update WHERE id=2 LIMIT 1");
        while(Aies_query.next())
        {
            qDebug() << "[sqlDebug] version from rpi_update :" << Aies_query.value(0).toString();
            QString strUpdateTemp("UPDATE rpi_update SET version='"+Aies_query.value(0).toString()+"', date='"+Aies_query.value(1).toString()+"', date_create='"+Aies_query.value(2).toString()+"', path='"+Aies_query.value(3).toString()+"', size='"+Aies_query.value(4).toString()+"' WHERE id=1");
            QSqlQuery Aies_update(strUpdateTemp);
        }
        QSqlQuery Aies_delete("DELETE FROM rpi_update WHERE id=2");
        qDebug() << "[switchDiffToPerma]: Changement de maj Diff en maj Perma";
    }
}

int CBdd::getIdleTime(QString mac)
{
    int idleTime = 0;
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getIdleTime]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT idleTime FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        idleTime = Aies_query.value(0).toInt();
        qDebug() << "idleTime: " << idleTime;
        return idleTime*1000*60; // en mn
    }
}

QString CBdd::getHstart(QString mac)
{
    QString Hstart = "HstartEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getHstart]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT hStart FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        Hstart = Aies_query.value(0).toString();
//        qDebug() << "Hstart: " << Hstart;
        return Hstart;
    }
}

QString CBdd::getHstop(QString mac)
{
    QString Hstop = "HstopEmpty";
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::getHstop]: BDD non ouverte");
        exit(1);
    }
    else
    {
        QSqlQuery Aies_query("SELECT hStop FROM pas WHERE mac = '"+mac+"'");
        Aies_query.next();
        Hstop = Aies_query.value(0).toString();
//        qDebug() << "Hstop: " << Hstop;
        return Hstop;
    }
}

void CBdd::setSliderUpdate(QDateTime dateActu)
{
    if(!Aies_bdd.isOpen())
    {
        qDebug() << "[CBdd::setSliderUpdate]: BDD non ouverte";
    }
    else
    {
        QSqlQuery Aies_query("SELECT id, date_start FROM slidezone WHERE date_start != '---' AND state = 'diff'");
        while(Aies_query.next())
        {
            QDateTime dateStart = QDateTime::fromString(Aies_query.value(1).toString(), "yyyyMMddHHmm");
            if(dateStart <= dateActu)
            {
                qDebug() << "ID des diapos devenant actives :" << Aies_query.value(0).toInt();
                QSqlQuery Aies_queryUpdate("UPDATE slidezone SET state = 'actif' WHERE id = "+Aies_query.value(0).toString());
            }
        }
        Aies_query.finish();
        QSqlQuery Aies_query2("SELECT id, date_stop FROM slidezone WHERE date_stop != '---' AND state = 'actif'");
        while(Aies_query2.next())
        {
            QDateTime dateStop = QDateTime::fromString(Aies_query2.value(1).toString(), "yyyyMMddHHmm");
            if(dateStop <= dateActu)
            {
                qDebug() << "ID des diapos devenant inactives :" << Aies_query.value(0).toInt();
                QSqlQuery Aies_queryUpdate2("UPDATE slidezone SET state = 'arch', date_stop = '---' WHERE id = "+Aies_query2.value(0).toString());
            }
        }
    }
}

QList<QList<QString> > CBdd::getActiveSlides(QString mac)
{
    bool newData = false;
    QString req = "SELECT id, path, time, priority FROM slidezone WHERE state = 'actif' AND (zone=0 OR zone="+getNoZone(mac)+") ORDER BY priority";

    QSqlQuery Aies_query(req);
    QList <QList<QString> > tabSlide;
    qDebug() << "CBdd::getActiveSlides " << req << " " << Aies_query.size() << " diapo(s) à afficher.";
    while(Aies_query.next())
    {
        newData = true;
        QList<QString> tempQList;
        tempQList.append(Aies_query.value(0).toString());
        tempQList.append(Aies_query.value(1).toString());
        tempQList.append(Aies_query.value(2).toString());
        tempQList.append(Aies_query.value(3).toString());
        tabSlide.append(tempQList);
    }
    if(newData == false)
    {
        QList<QString> tempQList;
        tempQList.append("none");
        tabSlide.append(tempQList);
    }
    return tabSlide;
}

QString CBdd::getActiveFlash()
{
    QSqlQuery Aies_query("SELECT content FROM flash WHERE id = 1");
    QString strFlash;
    while(Aies_query.next())
    {
        strFlash = Aies_query.value(0).toString();
    }
    return strFlash;
}

void CBdd::setCapteurs(QString temp, QString co2, QString cov, QString pres, QByteArray sd,QString mac)
{
    if(!Aies_bdd.isOpen())
    {
        qDebug() << "[CBdd::setCapteurs]: BDD non ouverte";
    }
    else
    {
        QSqlQuery Aies_query("UPDATE pas SET temp='"+temp+"', CO2='"+co2+
                             "', COV='"+cov+"', Pourcentage_SD ='"+sd+
                             "', presence='"+pres+"' WHERE mac='"+mac+"';");
    }
}

void CBdd::writeBdd(QString query)
{
    if(!Aies_bdd.isOpen())
    {
        qDebug("[CBdd::writeBdd]: BDD non ouverte");
        exit(1);
    }
    QSqlQuery Aies_query(query);
}

int CBdd::getUrgencyState()
{
    if(!Aies_bdd.isOpen()) {
        qDebug("[CBdd::getUrgencyState]: BDD non ouverte"); //verification d'accès à la bdd
        exit(1);
    } else {
         int urgencyIsOn = 0;
         QSqlQuery Aies_query("SELECT isOn FROM urgency"); //prend la valeur dans la BDD
         Aies_query.next();
         urgencyIsOn = Aies_query.value(0).toBool(); //transforme la valeur en bool
         //qDebug() << "[CBdd::getUrgencyState] Urgency: " << urgencyIsOn;
         return urgencyIsOn;
    } // else
}

QList<QString> CBdd::getImagesVideos(QString mac) {
    QList<QString> videoSlide;
    if(!Aies_bdd.isOpen()) {
        qDebug("[CBdd::getImagesVideos]: BDD non ouverte"); //verification d'accès à la bdd
        exit(1);
    } else {
        bool newData = false;
        QString req = "SELECT pathimg, date_create FROM slidezone WHERE state = 'actif' AND (zone=0 OR zone="+getNoZone(mac)+") " +
                      "AND pathimg IS NOT NULL";
        QSqlQuery Aies_query(req); // envoie de la requête à la BDD

        qDebug() << "[CBdd::getImagesVideos] obtention liste des images/vidéos " << req << " Nombre : " << Aies_query.size();
        while(Aies_query.next()) // la requête nous renvoie un resultat
        {
            newData = true;
            QString tempQList;
            tempQList.append(Aies_query.value(0).toString()); //on met les diapos dans une liste
            qDebug() << "[CBdd::getImagesVideos] Mise en liste de :" << Aies_query.value(0).toString();
            videoSlide.append(tempQList);
        } // wh

        if(newData == false) //aucune diapositive ne correspond à la requête
        {
            QString tempQList;
            tempQList.append("none");
            videoSlide.append(tempQList);
        } // if newdata
    } // else
    return videoSlide;
}

QByteArray CBdd::getPresCapteurs(QString mac)
{
    QByteArray cs;
    if(!Aies_bdd.isOpen()) {
        qDebug("[CBdd::getPresCapteurs]: BDD non ouverte"); //verification d'accès à la bdd
        exit(1);
    } else {
        QString req = "SELECT captPres, captQAir, captTemp, captFumee FROM pas WHERE mac="+mac+")";
        QSqlQuery Aies_query(req); // envoie de la requête à la BDD
        Aies_query.next();
        cs.append(Aies_query.value("captPres").toBool());
        cs.append(Aies_query.value("captQAir").toBool());
        cs.append(Aies_query.value("captTemp").toBool());
        cs.append(Aies_query.value("captFumee").toBool());
    } // else
    return cs;
} // creationCacheVideo

/*
void CBdd::getListePages(int zone)
{

}

*/
