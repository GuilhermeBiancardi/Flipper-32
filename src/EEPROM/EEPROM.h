#include <EEPROM.h>

class EEPROMManager {

public:

    /**
     * Construtor da classe EEPROMManager para inicializar a EEPROM.
     *
     * @param size O tamanho da EEPROM a ser inicializada em bytes.
     *
     * @return Nenhum.
     */
    EEPROMManager(int size) {
        EEPROM.begin(size);
    }

    /**
     * Função para limpar o conteúdo da EEPROM.
     *
     * @param size O tamanho da EEPROM a ser limpa em bytes.
     *
     * @return Nenhum.
     */
    void Clear(int size) {
        for (int nL = 0; nL < size; nL++) {
            EEPROM.write(nL, 0);
        }
        EEPROM.commit();
    }

    /**
     * Função para escrever uma string na EEPROM.
     *
     * @param address O endereço de memória da EEPROM onde a string será escrita.
     * @param value A string a ser escrita na EEPROM.
     *
     * @return Nenhum.
     */
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

    /**
     * Função para ler uma string da EEPROM.
     *
     * @param address O endereço de memória da EEPROM onde a leitura será iniciada.
     * @param sizeAdress O tamanho máximo da leitura em bytes.
     *
     * @return A string lida da EEPROM.
     */
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