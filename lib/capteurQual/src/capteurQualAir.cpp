/******************************************************************************
    Librairie pour la gestion du capteur SGP30

******************************************************************************/

#ifdef DEBUG_SERIAL
#endif

#define PROFONDEUR_HISTO 30

#include <Arduino.h>
#include <capteurQualAir.h>
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_BME280.h>

/******************************************************************************
    Constructeurs & destructeurs
******************************************************************************/
CapteurQualAir::CapteurQualAir()
{
    device_OK = 0;
}

CapteurQualAir::~CapteurQualAir()
{
}

/******************************************************************************
    Méthodes
******************************************************************************/

/// @brief 
/// @param _delayTime
void CapteurQualAir::init(unsigned long _delayTime)
{    
    uint8_t nbTest = 10;            // 10 essais

    delayTime = _delayTime;     // Sauvegarde le délai sélectionné
    device_OK = 0;              // état de la connexion au SGP30, état inversé
    lastDelay = 0 - delayTime;      //Première lecture

    //-------------------------------------------------------------------------
    // Démarrage du SGP30
    do
    {
#ifdef DEBUG_SERIAL
        Serial.println("Essai de connexion du SGP30");
#endif
        // Délai avant d'agir
        delay(200);
        // Teste la connexion avec le BME280
        device_OK = sgp.begin();
    } while (!device_OK && nbTest--);

    if (device_OK)
    {
#ifdef DEBUG_SERIAL
        Serial.println("Could not find a valid SGP30 sensor, check wiring!");
#endif
        return;
    }

#ifdef DEBUG_SERIAL
    Serial.print("Found SGP30 serial #");
    Serial.print(sgp.serialnumber[0], HEX);
    Serial.print(sgp.serialnumber[1], HEX);
    Serial.println(sgp.serialnumber[2], HEX);
#endif

    return;
}

/// @brief 
/// @param data_env_qualite 
/// @return 
uint8_t CapteurQualAir::lecture(sdata_env_qualite * data_env_qualite)
{
    if (millis() - lastDelay >= delayTime)        // toutes les secondes
    {
        lastDelay = millis();
        if (!sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
        }
        else
        {
            //-------- calcul filtre K = 0,5
            static uint16_t fn_1 = 0;

            data_env_qualite->eCO2 = (fn_1 >> 1) + (sgp.eCO2 >> 1);        // K = 0,5 donc décalage de 1 pour division par 2
            fn_1 = data_env_qualite->eCO2;
            data_env_qualite->TVOC = sgp.TVOC;

            if (!sgp.IAQmeasureRaw())
            {
#ifdef DEBUG_SERIAL
                Serial.println("Raw Measurement failed");
#endif
            }
            else
            {
                moyenneCO2();
            }
            // Serial.println();
        }
    }
    return true;
}

void CapteurQualAir::moyenneCO2()
{
#ifdef DEBUG_SERIAL
    Serial.print("Raw H2 ");
    Serial.print(sgp.rawH2);
    Serial.print(" | ");
    Serial.print("Raw Ethanol ");
    Serial.print(sgp.rawEthanol);
    Serial.print(" | ");

    counter++;
    if (counter == 30)
    {
        counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
        }
        else
        {
            static uint16_t histo_eCO2[PROFONDEUR_HISTO] = {0};
            static uint16_t histo_TCOV[PROFONDEUR_HISTO] = {0};
            static uint16_t pos_eCO2 = 0;
            static uint16_t pos_TCOV = 0;
            static uint16_t min_eCO2 = 0xFFFF;
            static uint16_t max_eCO2 = 0;
            static uint16_t min_TCOV = 0xFFFF;
            static uint16_t max_TCOV = 0;
            uint16_t diff_eCO2;
            uint16_t diff_TCOV;

            if (eCO2_base > max_eCO2) max_eCO2 = eCO2_base;
            if (TVOC_base > max_TCOV) max_TCOV = TVOC_base;

            if (eCO2_base < min_eCO2) min_eCO2 = eCO2_base;
            if (TVOC_base < min_TCOV) min_TCOV = TVOC_base;

            diff_eCO2 = eCO2_base - histo_eCO2[((pos_eCO2) ? (pos_eCO2 - 1) : (PROFONDEUR_HISTO - 1))];
            diff_TCOV = TVOC_base - histo_TCOV[((pos_TCOV) ? (pos_TCOV - 1) : (PROFONDEUR_HISTO - 1))];

            histo_eCO2[pos_eCO2++] = eCO2_base;
            if (pos_eCO2 == PROFONDEUR_HISTO)
            {
                pos_eCO2 = 0;
            }
            uint32_t moyenneCO2 = 0;
            uint8_t indices = 0;
            for (int i = 0; i < PROFONDEUR_HISTO; i++)
            {
                moyenneCO2 += histo_eCO2[i];
                if (histo_eCO2[i] != 0)
                    indices++;
            }
            moyenneCO2 /= indices;

            histo_TCOV[pos_TCOV++] = TVOC_base;
            if (pos_TCOV == PROFONDEUR_HISTO)
            {
                pos_TCOV = 0;
            }

            Serial.print(" Baseline values: eCO2: ");
            Serial.print(eCO2_base, DEC);
            Serial.print(" min: ");
            Serial.print(min_eCO2, DEC);
            Serial.print(" moy: ");
            Serial.print(moyenneCO2, DEC);
            Serial.print(" max: ");
            Serial.print(max_eCO2, DEC);
            Serial.print(" diff: ");
            Serial.print((int16_t)diff_eCO2, DEC);
            Serial.print(" & TVOC: ");
            Serial.print(TVOC_base, DEC);
            Serial.print(" diff: ");
            Serial.println((int16_t)diff_TCOV, DEC);
        }
    }
#endif
}

void CapteurQualAir::setHumidity(uint32_t absolute_humidity)
{
    sgp.setHumidity(absolute_humidity);
}
