/******************************************************************************
    Librairie pour la gestion de l'affichage du module QAir

    A base de SDD1306

******************************************************************************/

#include <Arduino.h>

#include <displayQAir.h>
#include <capteurEnv.h>
#include <capteurQualAir.h>

DisplayQAir::DisplayQAir(/* args */)
{}

DisplayQAir::~DisplayQAir()
{}

/// @brief Initialisation du SSD1306 / retourne ok si présent et démarré
void DisplayQAir::init()
{
    device_OK = 0;

    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    //-------------------------------------------------------------------------
    // Démarrage du oled SSD1306
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
#ifdef DEBUG_SERIAL
        Serial.println(F("SSD1306 allocation failed"));
#endif
        return;
    }

    device_OK = true;

#ifdef DEBUG_SERIAL
    Serial.println("-- Configuration du SSD1306 OK (OLED) --");
    Serial.println(F("SSD1306 config ok"));
#endif

    //******************************************************************
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display->display();
    delay(2000);        // Pause for 2 seconds

    // Clear the buffer
    display->clearDisplay();

    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display->setTextSize(1);                    // Normal 1:1 pixel scale
    display->setTextColor(SSD1306_WHITE);        // Draw white text
    display->setCursor(0, 0);                    // Start at top-left corner
    display->cp437(true);                        // Use full 256 char 'Code Page 437' font
    delay(2000);
    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...

    // Invert and restore display, pausing in-between
    display->invertDisplay(true);
    delay(1000);
    display->invertDisplay(false);
    delay(1000);

    return;
}

void DisplayQAir::displayAffiche(sdata_env data_env) 
{
#ifdef DEBUG_SERIAL
    Serial.printf("%0.2f°C, %0.2f %%HR, %0.0fhPa, %u m3\n", data_env.temperature, data_env.humidite, data_env.pression, data_env.hygroAbsolue);
#endif

    display->setTextSize(2);        // Double 2:1 pixel scale
    display->fillRect(0, 0, 128, 30, SSD1306_BLACK);
    display->setCursor(0, 0);
    display->print(data_env.temperature, 1);
    display->setCursor(84, 0);
    display->print(data_env.humidite, 0);
    display->setCursor(0, 16);
    display->print(data_env.pression, 0);
    display->setCursor(79, 16);
    display->print(data_env.hygroAbsolue);
    display->setTextSize(1);        // Normal 1:1 pixel scale
    display->setCursor(53, 1);
    display->print("°C");
    display->setCursor(110, 1);
    display->print("%HR");
    display->setCursor(48, 15);
    display->print("hPa");
    display->setCursor(110, 15);
    display->print("mg/");
    display->setCursor(110, 23);
    display->print(" m3");
    display->display();
}

void DisplayQAir::displayAffiche(sdata_env_qualite data_env_qualite)
{
#ifdef DEBUG_SERIAL
    Serial.printf("%u ppm eCO2, %u ppb TCOV\n", data_env_qualite.eCO2, data_env_qualite.TVOC);
#endif

    display->setTextSize(2);        // Normal 1:1 pixel scale
    display->fillRect(0, 31, 128, 32, SSD1306_BLACK);
    display->setCursor(0, 32);
    display->print(data_env_qualite.TVOC);
    display->setTextSize(1);        // Normal 1:1 pixel scale
    display->print("ppb TCOV");
    display->setCursor(0, 48);
    display->setTextSize(2);        // Normal 1:1 pixel scale
    display->print(data_env_qualite.eCO2);
    display->setTextSize(1);        // Normal 1:1 pixel scale
    display->print("ppm eCO2");
    display->display();
}
