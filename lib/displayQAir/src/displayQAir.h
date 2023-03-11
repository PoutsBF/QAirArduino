#pragma once 
/******************************************************************************
    Librairie pour la gestion de l'affichage du module QAir 

    A base de SDD1306

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_GFX.h>
// #include <FreeMono12pt7b.h>
#include <Adafruit_SSD1306.h>

#include <capteurEnv.h>
#include <capteurQualAir.h>

//---------------------------------------------------------
// Gestion du SSD1306
//
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL)
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

#define OLED_RESET 21              // Reset pin # (or -1 if sharing Arduino reset pin) @todo : modifier la patte !
#define SCREEN_ADDRESS 0x3C        ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

class DisplayQAir
{
private:
    Adafruit_SSD1306 *display;
    uint8_t device_OK;

    

public:
    DisplayQAir(/* args */);
    ~DisplayQAir();

    void init(void);
    void displayAffiche(sdata_env);
    void displayAffiche(sdata_env_qualite);
};
