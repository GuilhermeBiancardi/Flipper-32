#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class PN532Manager {

public:

    PN532Manager(uint8_t  IRQPin, uint8_t RESET): PN532(IRQPin, RESET) {}

    /**
     * Função de inicialização do programa.
     * Configura as configurações iniciais do hardware, como a inicialização do leitor NFC e impressão de informações da placa.
     */
    void Setup() {
        // Para Leonardo/Micro/Zero
        // while (!Serial) delay(10);

        // Inicia o leitor NFC
        PN532.begin();

        // Imprime informações da placa
        // HardwareInfos();
    }

    /**
     * Função principal do loop do programa.
     * Realiza a escrita e leitura de dados em uma Tag NFC, utilizando autenticação com chaves A e B.
     * Verifica a conexão com a Tag NFC, autentica o setor desejado e realiza operações de escrita e leitura.
     * É recomendado verificar se a Tag foi reconhecida e se o UID tem o tamanho correto antes de prosseguir com as operações.
     */
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
                CardToJson();
                // Escreve dados no bloco 4
                // WriteTag4Bytes(4, data);
                // Lê os dados do Bloco 4
                // ReadTag4Bytes(4);
                // Lê todos os dados disponíveis
                // TagDumpInfo4Bytes(KeyB);
            } else {
                // UID 7 bytes
                ReadTag7Bytes(4);
            }
        }

    }

    /**
     * Função para imprimir informações da placa PN532, incluindo o firmware.
     * Verifica se o leitor PN532 foi reconhecido corretamente e imprime as informações de versão do firmware.
     * Caso o leitor não seja encontrado, o programa entra em um loop infinito.
     */
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

    /**
     * Função para escrever dados em um bloco de uma tag MIFARE Classic com 4 bytes de tamanho.
     * Verifica se o bloco informado não é reservado para a Key do Setor e, caso não seja, escreve os dados no bloco.
     * @param block Número do bloco onde os dados serão escritos.
     * @param data Array de 17 bytes contendo os dados a serem escritos na tag. O tamanho deve ser de 17 bytes, sendo o último byte reservado para o caractere nulo (terminador de string).
     */
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

    /**
     * Função para ler dados de um bloco de uma tag MIFARE Classic com 4 bytes de tamanho.
     * @param block Número do bloco que contém os dados a serem lidos.
     */
    void ReadTag4Bytes(int block) {

        // Armazena dados da conexão
        uint8_t connection;

        // Armazena os dados lidos.
        uint8_t data[16];

        // Tenta ler o conteúdo do bloco informado
        connection = PN532.mifareclassic_ReadDataBlock(block, data);

        if (connection) {

            Serial.println("Dados contidos no bloco informado:");
            PN532.PrintHexChar(data, 16);
            Serial.println("");
            // Espera um pouco antes de ler o bloco novamente
            delay(1000);

        } else {
            Serial.println("Ooops ... Não foi possível autenticar o bloco. Tente outra Key.");
        }

    }

    /**
     * Função para ler dados de uma página de uma tag Mifare Ultralight com 7 bytes de UID.
     * @param page Número da página que contém os dados a serem lidos.
     */
    void ReadTag7Bytes(int page) {

        // Armazena dados da conexão
        uint8_t connection;

        if (uidLength == 7) {

            Serial.println("Tag Mifare Ultralight (7 byte UID)");

            // Tenta ler a página informada
            Serial.println("Lendo página.");
            uint8_t data[32];
            connection = PN532.mifareultralight_ReadPage(page, data);

            if (connection) {

                // Dados sendo lidos
                PN532.PrintHexChar(data, page);
                Serial.println("");
                // Espera um pouco antes de ler a página novamente
                delay(1000);

            } else {
                Serial.println("Ooops ... Não foi possível ler a página.");
            }

        }

    }

    /**
     * Função para fazer o dump de informações de uma tag Mifare Classic com 4 bytes de UID.
     * @param key Ponteiro para a chave de autenticação a ser usada para ler os blocos da tag.
     */
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

            // Verifica se este é um novo bloco para que possamos reautenticar
            if (PN532.mifareclassic_IsFirstBlock(currentblock)) {
                authenticated = false;
            }

            // Se o setor não tiver sido autenticado, faz isso primeiro
            if (!authenticated) {

                // Início de um novo setor ... tenta autenticar
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
                    Serial.println("Erro na autenticação.");
                }
            }

            // Se não estiver autenticado, pula o bloco
            if (!authenticated) {

                Serial.print("Bloco ");
                Serial.print(currentblock, DEC);
                Serial.println(" não foi possível autenticar.");

            } else {

                // Autenticado, lê o bloco
                connection = PN532.mifareclassic_ReadDataBlock(currentblock, data);

                if (connection) {

                    Serial.print("Bloco ");
                    Serial.print(currentblock, DEC);

                    if (currentblock < 10) {
                        Serial.print("  ");
                    } else {
                        Serial.print(" ");
                    }

                    PN532.PrintHexChar(data, 16);

                } else {
                    Serial.print("Bloco ");
                    Serial.print(currentblock, DEC);
                    Serial.println(" não foi possível ler o bloco.");
                }

            }

        }

    }

    void CardToJson() {
        uint8_t success = PN532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if (success) {
            
            String json = "{\"type\":\"NFC_TAG_4K\",\"uid\":\"" + byteToHexString(uid, uidLength) + "\",";
            
            for (int i = 0; i < 16; i++) {
                uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
                uint8_t block = (i * 4);
                uint8_t data[16];

                json += "\"sector_" + String(i) + "\":{";

                success = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, key);
                if (success) {
                    success = PN532.mifareclassic_ReadDataBlock(block, data);
                    if (success) {
                        String sectorKey = byteToHexString(key, sizeof(key));
                        String sectorData = byteToHexString(data, sizeof(data));
                        json += "\"key_a\":\"" + sectorKey + "\",";
                    }
                } else {
                    json += "\"key_a\":\"\\0\\0\\0\\0\\0\\0\",";
                }

                success = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, block, 1, key);
                if (success) {
                    success = PN532.mifareclassic_ReadDataBlock(block, data);
                    if (success) {
                        String sectorKey = byteToHexString(key, sizeof(key));
                        String sectorData = byteToHexString(data, sizeof(data));
                        json += "\"key_b\":\"" + sectorKey + "\",";
                    }
                } else {
                    json += "\"key_b\":\"\\0\\0\\0\\0\\0\\0\",";
                }

                for (int j = 0; j < 4; j++) {
                    block = ((i * 4) + j);
                    if(!IgnoreReservedBlocks(block)) {
                        success = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, key);
                        if (success) {
                            success = PN532.mifareclassic_ReadDataBlock(block, data);
                            if (success) {
                                String sectorKey = byteToHexString(key, sizeof(key));
                                String sectorData = byteToHexString(data, sizeof(data));
                                json += "\"block_" + String(block) + "\":{\"data\":\"" + sectorData + "\"},";
                            }
                        }
                    } else {
                        json += "\"block_" + String(block) + "\":{\"data\":\"ffffffffffff\"},";
                    }
                }

                json.remove(json.length() - 1); // remove a última vírgula
                json += "},";

            }

            json.remove(json.length() - 1); // remove a última vírgula
            json += "}";
            jsonString = json;

        }
    }

    String GetJSON() {
        return jsonString;
    }

    void SetJSON(String uid) {
        jsonString = uid;
    }


private:

    Adafruit_PN532 PN532;

    // Buffer para armazenar o UID retornado da Tag
    uint8_t uid[7] = { 0, 0, 0, 0, 0, 0, 0 };

    // Tamanho do UID da Tag (4 ou 7 bytes dependendo do tipo da Tag ISO14443A)
    uint8_t uidLength;

    // Todos os dados do cartão em json
    String jsonString;

    /**
     * Função que converte um conjunto bytes para HEX para String
    */
    String byteToHexString(uint8_t* data, size_t length) {
        String result = "";
        for (size_t i = 0; i < length; i++) {
            result += String(data[i] < 0x10 ? "0" : "") + String(data[i], HEX);
        }
        return result;
    }

    /**
     * Função para procurar e identificar uma tag ISO14443A usando o leitor PN532.
     *
     * @return True se uma tag foi encontrada, False caso contrário.
     */
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

    /**
     * Função para autenticar um bloco em uma tag Mifare Classic usando o leitor PN532.
     *
     * @param block O número do bloco a ser autenticado.
     * @param keyType O tipo de chave a ser utilizada para autenticação (0 para KeyA, 1 para KeyB).
     * @param key O array de 6 bytes contendo a chave a ser utilizada para autenticação.
     * @return True se a autenticação foi bem-sucedida, False caso contrário.
     */
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

            // Tenta autenticar o setor do bloco informado com a KeyA ou KeyB
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