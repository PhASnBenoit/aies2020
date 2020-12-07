#include "ci2c.h"

CI2c * CI2c::_singleton = nullptr;

CI2c::CI2c(QObject *parent, char noBus) :
    QObject(parent)
{
    _parent = parent;
    _noBus = noBus;
    _nbLink = 0;
} // constructeur

ssize_t CI2c::lire(uint8_t addr,  uint8_t *buffer, size_t lg)
{
    if(ioctl(_fileI2c, I2C_SLAVE, addr)!=0) {  // Règle le driver I2C sur l'adresse.
        QString mess="CI2c::lire Erreur ioctl acces au bus I2C";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if ioctl
    bzero(buffer, lg);
    QMutexLocker lock(&this->m_mutexI2c);  // verrouillage du mutex. Il est libéré en sortie de méthode

    ssize_t nb=read(_fileI2c, buffer, lg);
//    qDebug() << "CI2c:lire: " << buffer[0] << " " << buffer[1] << buffer[2] << " " << buffer[3] << buffer[4] << " " << buffer[5];
    return nb;
} // lire

ssize_t CI2c::ecrire(uint8_t addr, uint8_t *buffer, size_t lg)
{
    if(ioctl(_fileI2c, I2C_SLAVE, addr)!=0) {  // Règle le driver I2C sur l'adresse.
        QString mess="CI2c::ecrire Erreur ioctl acces au bus I2C";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if ioctl

    QMutexLocker lock(&this->m_mutexI2c);  // verrouillage du mutex. Il est libéré en sortie de méthode

    ssize_t nb=write(_fileI2c, buffer, lg);
//    qDebug() << "CI2c:ecrire: nb=" << nb << " : " << buffer[0] << " " << buffer[1] << buffer[2];
    return nb;
}

ssize_t CI2c::ecrirePuisLire(uint8_t addr, uint8_t *bufferW, uint16_t lgW, uint8_t *bufferR, uint16_t lgR)
{
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].len = lgW;
    msgs[0].buf = bufferW;

    msgs[1].addr = addr;
//    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART; // A VERIFIER !!!
    msgs[1].flags = 0;
    msgs[1].len = lgR;
    msgs[1].buf = bufferR;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = (lgR==0?1:2);

    if (ioctl(_fileI2c, I2C_RDWR, &msgset) < 0) {
        qDebug() << "ioctl(I2C_RDWR) in ecrirePuisLire";
        return -1;
    } // if

    return 0;
} // ecrirePuisLire

int CI2c::init()
{
    char filename[20];
    sprintf(filename, "/dev/i2c-%c",_noBus);
    if((_fileI2c=open(filename, O_RDWR))==-1) {  // ouvre le fichier virtuel d'accès à l'I2C
        QString mess="CI2c::init Erreur ouverture acces au bus I2C";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if open
    return _fileI2c;
} // init

int CI2c::getNbLink()
{
    return _nbLink;
} // getNbLink

CI2c *CI2c::getInstance(QObject *parent, char no)
{
    if (_singleton == nullptr)
    {
        qDebug("L'objet CI2c est créé");
        _singleton =  new CI2c(parent, no);
        _singleton->init();
        _singleton->_nbLink=1;
    }
    else
    {
        _singleton->_nbLink++;
        qDebug("singleton est déjà existant");
    }
    return _singleton;
} // getInstance

void CI2c::freeInstance()
{
    if (_singleton != nullptr)
      {
            _singleton->_nbLink--;
            if (_singleton->_nbLink==0) {
                close(_singleton->_fileI2c);
                delete _singleton;
                _singleton = nullptr;
            } // if mNbLink
      } // if null
} // freeInstance
