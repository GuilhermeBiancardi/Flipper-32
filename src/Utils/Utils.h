#include <SPI.h>
#include <WString.h>
#include <Print.h>

class UtilsManager {

public:

    UtilsManager() {}

    /**
     * Converte String para uint8_t* Array
     */
    void StringToUint8_t(String string, uint8_t* data) {
        char charArray[string.length() + 1];
        string.toCharArray(charArray, string.length() + 1);
        for (int i = 0; i < string.length(); i++) {
            data[i] = (uint8_t)charArray[i];
        }
    }

    /**
     * Converte uma HEX String para uint8_t* Array
     */
    void HexStringToUint8_t(String hexString, uint8_t* data) {
        // Converte a string em uma matriz de bytes
        for (int i = 0; i < hexString.length(); i += 2) {
            // Extrai cada par de caracteres da string
            String byteString = hexString.substring(i, i + 2);
            // Converte o par de caracteres em um byte hexadecimal
            data[i / 2] = strtoul(byteString.c_str(), NULL, 16);
        }
    }

    /**
     * Converte uma const HEX String para uint8_t Array
    */
    // void HexStringToUint8_t(const String& hexString, uint8_t* bytes) {
    //     // Garante que o comprimento da string hexadecimal seja par
    //     if (hexString.length() % 2 != 0) {
    //         return; // Saia se o comprimento for ímpar
    //     }
    //     // Converte cada par de caracteres hexadecimais em um byte
    //     for (size_t i = 0; i < hexString.length(); i += 2) {
    //         String hexPair = hexString.substring(i, i + 2);
    //         bytes[i / 2] = strtol(hexPair.c_str(), NULL, 16);
    //     }
    // }

    /**
     * Função que converte uint8_t* para HEX depois String
     */
    String Uint8_tToString(uint8_t* data, size_t length) {
        String result = "";
        for (size_t i = 0; i < length; i++) {
            result += String(data[i] < 0x10 ? "0" : "") + String(data[i], HEX);
        }
        return result;
    }

    /**
     * Coloca os caracteres de uma String em maiúsculo.
    */
    String toUpperCase(const String& str) {
        String upperCase;
        for (size_t i = 0; i < str.length(); i++) {
            upperCase += char(toupper(str[i]));
        }
        return upperCase;
    }

    /**
     * Função que converte uint16_t* em String.
    */
    String Uint16_tToString(uint16_t* data, size_t length, String separator = "") {
        String string = "";
        for (int i = 0; i < length; i++) {
            string += String(data[i]);
            if (i < length - 1) {
                string += separator;
            }
        }
        return string;
    }

    /**
     * Função que converte uint16_t em String.
    */
    String Uint16_tToString(uint16_t data) {
        return String(data);
    }

    /**
     * Função que converte uint32_t em String.
    */
    String Uint32_tToString(uint32_t data) {
        return String(data);
    }

};