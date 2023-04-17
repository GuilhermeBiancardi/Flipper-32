#include <EEPROM.h>

class EEPROMManager {

public:

    EEPROMManager(int size) {
        EEPROM.begin(size);
    }

    // Limpeza da EEPROM
    void Clear(int size) {
        for (int nL = 0; nL < size; nL++) {
            EEPROM.write(nL, 0);
        }
        EEPROM.commit();
    }

    void WriteStr(int address, String value) {

        unsigned int nL;

        for (nL = 0; nL < value.length(); nL++) {
            EEPROM.write(address + nL, value.charAt(nL));

            if (nL >= 1000) {
                break;
            }
        }

        if (nL < 1000) {
            EEPROM.write(address + nL, (char)0);
        }

        EEPROM.commit();

    }

    String ReadStr(int address, int sizeAdress) {

        String readStr = "";
        char readByte;
        int readAddress = address;

        do {
            readByte = EEPROM.read(readAddress);
            if ((readByte != (char)NULL && readByte != (char)0xFF)) {
                readStr += readByte;
            }
            readAddress++;
        } while ((readAddress < (address + sizeAdress)));

        // readStr = readStr.substring(0, readStr.length() - 1);

        return readStr;

    }

};