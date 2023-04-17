#include <Arduino.h>

#include "EEPROM/EEPROM.h"
#include "PN532/PN532.h"
#include "WebService/WebService.h"
#include "SDCard/SDCard.h"

PN532Manager PN532(21, 22);
SDCardManager SDCard(5);

void setup(void) {

    Serial.begin(9600);

    PN532.Setup();
    WebServiceSetup();
    SDCard.Setup();

}

void loop(void) {
    PN532.Loop();
}