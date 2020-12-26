#include "cgpio.h"

CGpio::CGpio(int addr)
{
    mAddr = addr;
    gpioExport();
    init();
} // constructeur

CGpio::~CGpio()
{
  gpioUnexport();
} // destructeur

int CGpio::init()
{
    usleep(50000);
    sprintf(mFilename,"/sys/class/gpio/gpio%d/",mAddr);
    QString command = "sudo chmod -R 777 "+QString(mFilename);
    QProcess *proc= new QProcess(this);
    proc->start(command);
    //sleep(1);
    return 1;
}

int CGpio::gpioUnexport()
{
    int fd;
    char buffer[10];

    sprintf(mFilename,"/sys/class/gpio/unexport");
    fd = open(mFilename, O_WRONLY);
    if (fd < 0)
    {
        qDebug() << "CGpio::gpioUnexport: Erreur d'ouverture du fichier " << mFilename;
        emit erreur("CGpio::gpioUnexport: Erreur export GPIO !");
    } // if erreur open
    sprintf(buffer,"%d", mAddr);
    if( write(fd, buffer, strlen(buffer)) != int(strlen(buffer)))
    {
        emit erreur("CGPIO::gpioUnexport: Erreur write GPIO");
    }
    close(fd);
    return 0;
}

int CGpio::gpioDirection(int dir)
{
    int fd;
    char buffer[10];

    sprintf(mFilename,"/sys/class/gpio/gpio%d/direction", mAddr);
    fd = open(mFilename, O_WRONLY);
    if (fd < 0)
    {
        emit erreur("CGPIO::gpioDirection: Erreur direction GPIO !");
    } // if erreur open
    if (dir==0)
      strcpy(buffer,"in");
    else
      strcpy(buffer,"out");

    if( write(fd, buffer, strlen(buffer)) != int(strlen(buffer)))
    {
        emit erreur("CGPIO::gpioDirection: Erreur write GPIO");
    }
    close(fd);
    return 0;
}

int CGpio::gpioExport()
{
    int fd;
    char buffer[10];

    sprintf(mFilename,"/sys/class/gpio/export");
    fd = open(mFilename, O_WRONLY);
    if (fd < 0)
    {
        emit erreur("CGPIO::gpioExport: Erreur export GPIO !");
    } // if erreur open
    sprintf(buffer,"%d", mAddr);
    int nb=write(fd, buffer, strlen(buffer));
    if (nb != static_cast<int>(strlen(buffer)))
    {
        emit erreur("CGPIO::gpioExport: Erreur write GPIO");
    }
    close(fd);
    return 0;
}
	

int CGpio::lire()
{
    int fd;
    char buffer[10];

    gpioDirection(0);
    sprintf(mFilename,"/sys/class/gpio/gpio%d/value", mAddr);
    fd = open(mFilename, O_RDONLY);
    if (fd < 0) {
        emit erreur("CGPIO::lire: Erreur export GPIO !");
    } // if erreur open

    if( read(fd, buffer, sizeof(buffer)) < 1)
    {
        emit erreur("CGPIO::lire: Erreur read GPIO");
    }
    close(fd);

    return atoi(buffer);
}
	
int CGpio::ecrire(int value)
{
    int fd;
    char buffer[2]={'0','1'};

    gpioDirection(1);
    sprintf(mFilename,"/sys/class/gpio/gpio%d/value", mAddr);
    fd = open(mFilename, O_WRONLY);
    if (fd < 0)
    {
        emit erreur("CGPIO::ecrire: Erreur export GPIO !");
    } // if erreur open

    if (write(fd, &buffer[(value==0?0:1)], 1) != 1)
    {
        emit erreur("CGPIO::ecrire: Erreur write GPIO");
    }
    close(fd);
    return(0);
}	
