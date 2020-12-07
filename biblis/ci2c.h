#ifndef CI2C_H
#define CI2C_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <stdio.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <strings.h>

// Terrible portability hack between arm-linux-gnueabihf-gcc on Mac OS X and native gcc on raspbian.
#ifndef I2C_M_RD
    #include <linux/i2c.h>
#endif

class CI2c : public QObject
{
    Q_OBJECT

public:
    // creation destruction de l'objet
    static CI2c *getInstance(QObject *parent = nullptr, char no = '1');
    static void freeInstance();

    ssize_t lire(uint8_t addr, uint8_t *buffer, size_t lg);
    ssize_t ecrire(uint8_t addr, uint8_t *buffer, size_t lg);
    ssize_t ecrirePuisLire(uint8_t addr, uint8_t *bufferW, uint16_t lgW, uint8_t *bufferR, uint16_t lgR);
    int init();
    int getNbLink();
    QMutex m_mutexI2c;

private:
    explicit CI2c(QObject *parent = nullptr, char noBus = '1');
    QObject *_parent;
    //int _addr;   // Adresse du composant I2C
    char _noBus;   // No d'accès au fichier /dev
    int _fileI2c;  // descripteur du fichier i2C
    int _nbLink;
    static CI2c *_singleton;

signals:
    void sigErreur(QString msg);
};

#endif // CI2C_H
