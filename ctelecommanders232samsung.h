#ifndef CTELECOMMANDERS232SAMSUNG_H
#define CTELECOMMANDERS232SAMSUNG_H

#include <QObject>
#include <QDebug>
#include <unistd.h>  // usleep
#include "crs232c.h"

//#define ERREUR -1
#define DIFFUSION 0xFE
#define ENTETE 0xAA
//#define OK 0
#define ON 1
#define OFF 0

typedef enum sources {
    PC=0x14,
    DVI=0x18,
    SOURCE_IN=0x0C,
    COMPOSANT=0x08,
    MAGIC_INFO=0x20,
    DVI_VIDEO=0x1F,
    HDMI1=0x21,
    HDMI_PC,
    HDMI2,
    HDMI2_PC,
    HDBT=0x55,
    INCONNU=-1
} E_SOURCE_ENTREE;

typedef enum etat {
    DEBUT_TRAME=0,
    CORPS_TRAME,
    FIN_TRAME
} E_ETAT;

class CTelecommandeRs232Samsung : public QObject
{
    Q_OBJECT

public:
    explicit CTelecommandeRs232Samsung(QObject *parent = nullptr, const QString &nomRs232 = "/dev/ttyUSB0");
    ~CTelecommandeRs232Samsung();

    // contrôle de l'alimentation
    int setAlimentation(int id, bool alim);
    bool getEtatAlimentation(int id);
    int setSourceEntree(int id, E_SOURCE_ENTREE src);
    int getSourceEntree(int id);
    E_SOURCE_ENTREE getCodeSource(QString nomSource);
    QString getSource(E_SOURCE_ENTREE numSource);

signals:
    void sigErreur(char noErr, QString mess);
    void sigAffTrame(QString);
    void sigEtatAlimentation(char etat);
    void sigSourceEntree(char src);

private slots:
    void onErreur(QSerialPort::SerialPortError noErr);
    void onReponse(unsigned char octet0, unsigned char octet1, unsigned char octet4, unsigned char octet6);

private:
    CRs232c *rs;
    char mTrame[50];
    char mRep[50];

    char getSommeDeControle(int lg);
    int setId(int id);
    int ouvrirRs232(const QString &nomRs232);
    void debugAffiche(int lg);
    void sendErreurTc(char noErr, QString lieu);
};

#endif // CTELECOMMANDERS232SAMSUNG_H
