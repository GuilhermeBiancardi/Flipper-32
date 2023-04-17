#include <Arduino.h>

#include "EEPROM/EEPROM.h"
#include "PN532/PN532.h"
#include "WebService/WebService.h"
#include "SDCard/SDCard.h"
#include "InfraRed/InfraRed.h"

PN532Manager PN532(21, 22);
SDCardManager SDCard(5);
InfraRed IR;

void setup(void) {

    Serial.begin(9600);

    PN532.Setup();
    SDCard.Setup();
    IR.Setup();
    
    WebServiceSetup();

}

void loop(void) {
    // PN532.Loop();
    IR.Loop();
}