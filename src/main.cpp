#include <Arduino.h>
#include <vector>

/**
 * 0 = Standby
 * 1 = IR
 * 2 = NFC Read
 * 3 = NFC Write
*/
int SystemMode = 0;

#include "Utils/Utils.h"
UtilsManager Utils;

#include "EEPROM/EEPROM.h"
EEPROMManager InternalMemory;

#include "SDCard/SDCard.h"
SDCardManager SDCard(5);

#include "PN532/PN532.h"
PN532Manager PN532(21, 22);

#include "InfraRed/InfraRed.h"
InfraRedManager IR(A4, A7);

#include "WebService/WebService.h"

void setup(void) {
    Serial.begin(9600);
    InternalMemory.Setup();
    PN532.Setup();
    SDCard.Setup();
    IR.Setup();
    WebServiceSetup();
}

void loop(void) {
    WebServiceLoop();
    if (SystemMode == 1) {
        IR.Loop();
    } else if (SystemMode == 2 || SystemMode == 3) {
        PN532.Loop();
    }
}

// ----------------- Exemplo Callback --------------------

// void teste(int number, void (*callback)(String)) {
//     for (int i = 0; i < number; i++) {
//         delay(10);
//         Serial.println(i);
//     }
//     if (callback != 0) {
//         callback("Processo concluído com sucesso!");
//     }
// }
//
// teste(100, [](String teste) {
//     Serial.println(teste);
// });
//
// teste(100);