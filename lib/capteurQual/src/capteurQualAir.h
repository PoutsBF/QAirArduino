#pragma once 
/******************************************************************************
Librairie pour la gestion du capteur SGP30

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_SGP30.h>


//---------------------------------------------------------
// Fonctions et variables pour la gestion du capteur SGP30
//
struct sdata_env_qualite
{
    uint16_t eCO2;
    uint16_t TVOC;
};

class CapteurQualAir
{
private:
    Adafruit_SGP30 sgp;
    uint8_t device_OK;
    void moyenneCO2(void);

    unsigned long delayTime;        // délai entre les mesures
    unsigned long lastDelay;        // Timer pour les délais entre mesures

public:
    CapteurQualAir();
    ~CapteurQualAir();

    void init(unsigned long _delayTime);
    uint8_t lecture(sdata_env_qualite * data_env_qualite);
    void setHumidity(uint32_t absolute_humidity);
};
