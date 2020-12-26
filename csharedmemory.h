#ifndef CSHAREDMEMORY_H
#define CSHAREDMEMORY_H

#include <QObject>
#include <QDebug>
#include <QSharedMemory>

#define KEY "./aies.ini"

typedef struct s_capteurs {
    int mesTVOC;
    int mesECO2;
    int mesRawH2;
    int mesRawEthanol;
    int mesBaselineTVOC;
    int mesBaselineECO2;
    bool capteurGazFumee;
    bool capteurPresence;
    float mesTemp;
    bool capteurUTv;
} T_CAPTEURS;

class CSharedMemory : public QObject
{
    Q_OBJECT
public:
    explicit CSharedMemory(QObject *parent = nullptr);
    ~CSharedMemory();
    int getMesTVOC() const;
    int getMesECO2() const;
    int getMesRawH2() const;
    int getMesRawEthanol() const;
    int getMesBaselineTVOC() const;
    int getMesBaselineECO2() const;
    bool getCapteurGazFumee() const;
    bool getCapteurPresence() const;
    float getMesTemp() const;
    bool getCapteurUTv() const;

    void setMesTVOC(int val);
    void setMesECO2(int val);
    void setMesRawH2(int val);
    void setMesRawEthanol(int val);
    void setMesBaselineTVOC(int val);
    void setMesBaselineECO2(int val);
    void setCapteurGazFumee(bool val);
    void setCapteurPresence(bool val);
    void setMesTemp(float val);
    void setCapteurUTv(bool value);

private:
    QSharedMemory *shm;
    T_CAPTEURS *mem;

signals:

public slots:
};

#endif // CSHAREDMEMORY_H
