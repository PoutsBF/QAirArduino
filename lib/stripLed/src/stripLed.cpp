/******************************************************************************
Librairie pour la gestion ddes néoleds

******************************************************************************/

#include <Arduino.h>

#include <stripLed.h>

/// @brief Initialise les néopixels
StripLed::StripLed(/* args */)
{}

StripLed::~StripLed()
{}

void StripLed::miseAJour()
{
    static uint8_t q = 0;

    if (millis() - tempo > delta)
    {
        tempo = millis();

        if (q >= (strip.numPixels() - nbLedStrip))
            q = 0;
        else
            q++;

        for (uint16_t i = 0; i < strip.numPixels(); i += 1 + strip.numPixels() - nbLedStrip)
        {
            strip.setPixelColor(i + q, couleurStrip);        // turn every third pixel on
        }
        strip.show();
    }
}

/// @brief
/// @param WheelPos
/// @return
uint32_t StripLed::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void StripLed::init()
{
    strip.setPin(15);        // A MODIFIER !!
    strip.updateType(NEO_GRBW + NEO_KHZ800);
    strip.updateLength(8);

    strip.begin();                  // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();                   // Turn OFF all pixels ASAP
    strip.setBrightness(50);        // Set BRIGHTNESS to about 1/5 (max = 255)

    delta = 500;
    tempo = 0 - delta;

    couleurStrip = 0;
    nbLedStrip = 0;

    device_ok = true;
}

/// @brief Fonction pour la couleur en fonction du niveau de CO2
/// @param eCO2
void StripLed::afficheStrip(uint16_t eCO2)
{
    strip.clear();

    if (eCO2 < 750)
    {
        delta = 1000;
        couleurStrip = 0x00FF00;
        nbLedStrip = 1;
    }
    else if (eCO2 < 1000)
    {
        delta = 900;
        couleurStrip = 0x3FDC04;
        nbLedStrip = 2;
    }
    else if (eCO2 < 1250)
    {
        delta = 800;
        couleurStrip = 0x79bd08;
        nbLedStrip = 3;
    }
    else if (eCO2 < 1500)
    {
        delta = 800;
        couleurStrip = 0xB39e0c;
        nbLedStrip = 4;
    }
    else if (eCO2 < 2000)
    {
        delta = 700;
        couleurStrip = 0xEd7f10;
        nbLedStrip = 5;
    }
    else if (eCO2 < 2500)
    {
        delta = 700;
        couleurStrip = 0xF44708;
        nbLedStrip = 6;
    }
    else if (eCO2 < 3000)
    {
        delta = 600;
        couleurStrip = 0xf92805;
        nbLedStrip = 7;
    }
    else
    {
        delta = 500;
        couleurStrip = 0xFF0000;
        nbLedStrip = 7;
    }
}
