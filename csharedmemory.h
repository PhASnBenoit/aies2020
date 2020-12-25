#ifndef CSHAREDMEMORY_H
#define CSHAREDMEMORY_H

#include <QObject>
#include <QDebug>
#include <QSharedMemory>

#define KEY "./aies.ini"

typedef struct s_capteurs {
    int mesH2;
    int mesEthanol;
    int mesRawTVOC;
    int mesRawECO2;
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
    int getMesH2() const;
    int getMesEthanol() const;
    int getMesRawTVOC() const;
    int getMesRawECO2() const;
    int getMesBaselineTVOC() const;
    int getMesBaselineECO2() const;
    bool getCapteurGazFumee() const;
    bool getCapteurPresence() const;
    float getMesTemp() const;

    void setMesH2(int val) const;
    void setMesEthanol(int val) const;
    void setMesRawTVOC(int val) const;
    void setMesRawECO2(int val) const;
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
