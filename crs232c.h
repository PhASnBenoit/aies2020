#ifndef CRS232C_H
#define CRS232C_H

#include <QSerialPort>
#include <QTimer>
#include <QDebug>
#include <QThread>

#define ERREUR -1
#define OK 0
#define TO 3000  // 3s timeout réception

class CRs232c : public QObject
{
    Q_OBJECT

public:
    explicit CRs232c(QObject *parent = nullptr, const QString &nomPort = "/dev/ttyUSB0");
    ~CRs232c();
    int initialiser(QSerialPort::BaudRate vitesse, QSerialPort::DataBits data,
                    QSerialPort::Parity parity, QSerialPort::StopBits nbStop,
                    QSerialPort::FlowControl flow);
    int ouvrirPort();
    //char lire(char *trame, int nbOctets);
    int ecrire(char *trame, int nbOctets);

private:
    QSerialPort *mSp;
    bool mRec;  // état pour purger les caractères parasites

signals:
    void sigErreur(QSerialPort::SerialPortError err);
    void sigReponse(unsigned char octet0, unsigned char octet1, unsigned char octet4, unsigned char octet6);

public slots:
    void onReadyRead();
    void onErreur(QSerialPort::SerialPortError err);
};

#endif // CRS232C_H
