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

    void setMesTVOC(int val) const;
    void setMesECO2(int val) const;
    void setMesRawH2(int val) const;
    void setMesRawEthanol(int val) const;
    void setMesBaselineTVOC(int val) const;
    void setMesBaselineECO2(int val) const;
    void setCapteurGazFumee(bool val) const;
    void setCapteurPresence(bool val) const;
    void setMesTemp(float val) const;
private:
    QSharedMemory *shm;
    T_CAPTEURS *capt;

signals:

public slots:
};

#endif // CSHAREDMEMORY_H
