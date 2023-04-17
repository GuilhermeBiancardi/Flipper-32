#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class PN532Manager {

public:

    PN532Manager(uint8_t  IRQPin, uint8_t RESET): PN532(IRQPin, RESET) {}

    void Setup() {

        // Para Leonardo/Micro/Zero
        // while (!Serial) delay(10);

        // Inicia o Leitor
        PN532.begin();

        // Imprime dados da placa
        HardwareInfos();

    }

    void Loop() {

        // 16 Bytes (caracteres) por bloco
        uint8_t data[17] = { "@GuilhermeAw.com" };

        // KeyA chave padrão
        uint8_t KeyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

        // KeyB chave padrão
        uint8_t KeyB[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

        // Verifica se a Tag foi reconhecida.
        if (TagConnection()) {
            // Verifica se o UID tem 4 bytes e verifica se o setor do bloco informado foi autenticado
            if (BlockConnection(4, 0, KeyA)) {
                // Escreve dados no bloco 4
                WriteTag4Bytes(4, data);
                // Lê os dados do Bloco 4
                ReadTag4Bytes(4);
                // Lê todos os dados disponíveis
                TagDumpInfo4Bytes(KeyB);
            } else {
                // UID 7 bytes
                ReadTag7Bytes(4);
            }
        }

        delay(2000);

    }

    void HardwareInfos() {

        // Armazena dados de versão se o leitor for reconhecido.
        uint32_t versiondata = PN532.getFirmwareVersion();
        if (!versiondata) {
            Serial.print("Não foi possível encontrar uma placa PN53x");
            while (1);
        }

        // Tudo OK, Imprime algumas informações da placa.
        Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
        Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
        Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

    }

    void WriteTag4Bytes(int block, uint8_t data[17]) {

        // Armazena dados da conexão
        uint8_t connection;

        if (!IgnoreReservedBlocks(block)) {

            connection = PN532.mifareclassic_WriteDataBlock(4, data);

            if (connection) {
                Serial.println("Dados armazenados no bloco informado.");
            } else {
                Serial.println("Ooops ... Não foi possível escrever no bloco.");
            }

        } else {
            Serial.println("O bloco informado é reservado para a Key do Setor.");
        }

    }

    void ReadTag4Bytes(int block) {

        // Armazena dados da conexão
        uint8_t connection;

        // Armazena os dados lidos.
        uint8_t data[16];

        // Tenta ler o conteúdo do bloco 4
        connection = PN532.mifareclassic_ReadDataBlock(block, data);

        if (connection) {

            Serial.println("Dados contidos no bloco informado:");
            PN532.PrintHexChar(data, 16);
            Serial.println("");
            // Espero um pouco antes de ler o bloco novamente
            delay(1000);

        } else {
            Serial.println("Ooops ... Não foi possível autenticar o bloco. Tente outra Key.");
        }

    }

    void ReadTag7Bytes(int page) {

        // Armazena dados da conexão
        uint8_t connection;

        if (uidLength == 7) {

            Serial.println("Tag Mifare Ultralight (7 byte UID)");

            // Tenta ler a página 4
            Serial.println("Lendo página.");
            uint8_t data[32];
            connection = PN532.mifareultralight_ReadPage(page, data);

            if (connection) {

                // Dados sendo lidos
                PN532.PrintHexChar(data, page);
                Serial.println("");
                // Espero um pouco antes de ler a página novamente
                delay(1000);

            } else {
                Serial.println("Ooops ... Não foi possível ler a página.");
            }

        }

    }

    void TagDumpInfo4Bytes(uint8_t* key) {

        // Armazena dados da conexão
        uint8_t connection;

        // Bloco Atual
        uint8_t currentblock;

        // Armazena se o bloco está autenticado
        bool authenticated = false;

        // Armazena os dados do bloco
        uint8_t data[16];

        for (currentblock = 0; currentblock < 64; currentblock++) {

            // Check if this is a new block so that we can reauthenticate
            if (PN532.mifareclassic_IsFirstBlock(currentblock)) {
                authenticated = false;
            }

            // Se o setor não tiver sido autenticado, faz isso primeiro
            if (!authenticated) {

                // Starting of a new sector ... try to to authenticate
                Serial.print("----------------------- Setor ");
                Serial.print(currentblock / 4, DEC);
                Serial.println("------------------------");

                // Chave padrão KeyA (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF) para Mifare Classic
                // Ou KeyA (0xA0 0xA1 0xA2 0xA3 0xA4 0xA5) para NDEF Tags,
                // Mas o KeyB é o mesmo para os 2 (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF)
                connection = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, key);

                if (connection) {
                    authenticated = true;
                } else {
                    Serial.println("Erro na authenticação.");
                }
            }

            // Se não estiver autenticado, pula o bloco
            if (!authenticated) {

                Serial.print("Block ");
                Serial.print(currentblock, DEC);
                Serial.println(" não foi possível autenticar.");

            } else {

                // Autenticado, lê o bloco
                connection = PN532.mifareclassic_ReadDataBlock(currentblock, data);

                if (connection) {

                    Serial.print("Block ");
                    Serial.print(currentblock, DEC);

                    if (currentblock < 10) {
                        Serial.print("  ");
                    } else {
                        Serial.print(" ");
                    }

                    PN532.PrintHexChar(data, 16);

                } else {
                    Serial.print("Block ");
                    Serial.print(currentblock, DEC);
                    Serial.println("  não foi possível ler o bloco.");
                }

            }

        }

    }

private:
    
    Adafruit_PN532 PN532;

    // Buffer para armazenar o UID retornado da Tag
    uint8_t uid[7] = { 0, 0, 0, 0, 0, 0, 0 };

    // Tamanho do UID da Tag (4 ou 7 bytes dependendo do tipo da Tag ISO14443A)
    uint8_t uidLength;

    bool TagConnection() {

        // Armazena dados da conexão
        uint8_t connection;

        // Aguarda uma Tag ISO14443A (Mifare, etc.) ser encontrada pela placa.
        // Quando encontrada armazenará o UID da Tag na variável uidLength
        // Se o UID tiver 4 bytes = (Mifare Classic) ou 7 bytes = (Mifare Ultralight)
        connection = PN532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

        if (connection) {

            // Imprime informações da Tag encontrada.
            Serial.println("Encontrado Tag ISO14443A");
            Serial.print("-- Tamanho da UID: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
            Serial.print("-- Valor da UID: ");
            PN532.PrintHex(uid, uidLength);
            Serial.println("");

            return true;

        } else {

            return false;

        }

    }

    bool BlockConnection(int block, int keyType, uint8_t* key) {

        // Armazena dados da conexão
        uint8_t connection;

        if (uidLength == 4) {

            Serial.println("Tag Mifare Classic (4 byte UID)");

            if (keyType == 0) {
                Serial.println("Tentando autenticar o bloco informado com a chave padrão KeyA.");
            }

            if (keyType == 1) {
                Serial.println("Tentando autenticar o bloco informado com a chave padrão KeyB.");
            }

            // Tenta autenticar o setor do bloco informado com a KeyA
            connection = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, block, keyType, key);

            if (connection) {
                Serial.println("Bloco autenticado.");
                return true;
            } else {
                Serial.println("Erro na autenticação.");
                return false;
            }

        } else {
            return false;
        }

    }

    // Ignora os blocos que contém as chaves de acesso e as informações do fabricante.
    bool IgnoreReservedBlocks(int AtualBlock) {
        bool found = false;
        // Blocos reservados
        int array[] = { 0, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63 };
        for (int i = 0; i < 17; i++) {
            if (array[i] == AtualBlock) {
                found = true;
                break;
            }
        }
        return found;
    }

};