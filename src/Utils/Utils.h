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
     * Função que converte uint16_t* Array em String.
    */
    String Uint16_tArrayToString(uint16_t* data, size_t length, String separator = ",") {
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
     * Função que converte String em uint16_t.
    */
    uint16_t StringToUint16_t(String data) {
        return data.toInt();
    }

    /**
     * Função que converte String em uint16_t Array.
    */
    uint16_t* StringToUint16_tArray(String str, size_t length, String separator = ",") {
        size_t separatorLength = separator.length();
        size_t startIndex = 0;
        size_t nextIndex = 0;
        size_t arrayIndex = 0;

        // Counting the number of elements in the string
        length = 1;
        nextIndex = str.indexOf(separator);
        while (nextIndex != -1) {
            length++;
            nextIndex = str.indexOf(separator, nextIndex + separatorLength);
        }

        // Allocate memory for the array
        uint16_t* dataArray = new uint16_t[length];

        // Parsing the string and filling the array
        nextIndex = 0;
        while (arrayIndex < length && nextIndex != -1) {
            nextIndex = str.indexOf(separator, startIndex);
            String token = str.substring(startIndex, nextIndex != -1 ? nextIndex : str.length());
            dataArray[arrayIndex++] = token.toInt();
            startIndex = nextIndex != -1 ? nextIndex + separatorLength : nextIndex;
        }

        return dataArray;
    }

    /**
     * Função que converte String em uint16_t*.
    */
    uint16_t* StringToUint16_tArray(String data) {
        uint16_t number = data.toInt();
        uint16_t* array = reinterpret_cast<uint16_t*>(&number);
        return array;
    }

    /**
     * Função que converte uint32_t em String.
    */
    String Uint32_tToString(uint32_t data) {
        return String(data);
    }

    /**
     * Função que converte String em uint32_t.
    */
    uint32_t StringToUint32_t(String data) {
        return data.toInt();
    }

    /**
     * Função que converte String em uint32_t*.
    */
    uint32_t* StringToUint32_tArray(String data) {
        uint32_t number = data.toInt();
        uint32_t* array = reinterpret_cast<uint32_t*>(&number);
        return array;
    }

};