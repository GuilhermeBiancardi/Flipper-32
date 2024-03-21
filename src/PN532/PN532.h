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

        // Modo Leitura
        if (SystemMode == 2) {
            // Verifica se a Tag foi reconhecida.
            if (TagConnection()) {
                CardToJson();
            }
        }

        // Verifica se o UID tem 4 bytes e verifica se o setor do bloco informado foi autenticado
        // if (BlockConnection(4, 0, KeyA)) {
        //     // Escreve dados no bloco 4
        //     WriteTag4Bytes(4, data);
        //     // Lê os dados do Bloco 4
        //     ReadTag4Bytes(4);
        //     // Lê todos os dados disponíveis
        //     TagDumpInfo4Bytes(KeyB);
        // } else {
        //     // UID 7 bytes
        //     ReadTag7Bytes(4);
        // }

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
        uint8_t success = TagConnection();
        if (success) {
            std::vector<String> keys = SDCard.FileReadLineByLine("System/KEYS/keys.list");
            String json = "{\"type\":\"NFC_TAG_4K\",\"uid\":\"" + byteToHexString(uid, uidLength) + "\",";
            for (int i = 0; i < 16; i++) {
                uint8_t key[6], block = (i * 4);
                std::vector<String> keyData = OrganizeAuthenticateData(keys, i, block);
                keyData[0] != 0 ? hexToMatriz(keyData[0], key) : void();
                keyData[1] != 0 ? hexToMatriz(keyData[1], key) : void();
                json += keyData[3];
                for (int j = 0; j < 4; j++) {
                    uint8_t data[16], actualBlock = block + j;
                    success = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, actualBlock, keyData[2].toInt(), key);
                    if (success) {
                        success = PN532.mifareclassic_ReadDataBlock(actualBlock, data);
                        json += "\"block_" + String(actualBlock) + "\":{\"data\":\"" + byteToHexString(data, sizeof(data)) + "\"},";
                    } else {
                        json += "\"block_" + String(actualBlock) + "\":{\"data\":\"\"},";
                    }
                }
                json.remove(json.length() - 1);
                json += "},";
            }
            json.remove(json.length() - 1);
            json += "}";
            jsonString = json;
        }
    }

    String GetJSON() {
        return jsonString;
    }

    void SetJSON(String json) {
        jsonString = json;
    }

    String StartWriteData(String string, int block, int keyType, String key, bool keySave = false) {

        if (SystemMode == 3) {

            /**
             * Checa a existência de uma tag antes de executar a gravação.
            */
            TagConnection();

            bool isWrite = false;

            // 16 Bytes (caracteres) por bloco + 1 byte nulo
            uint8_t data[17] = { "" };

            // 16 Bytes key
            uint8_t newKeySector[16] = { "" };

            // KeyA/B padrão
            uint8_t keySector[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

            // Verifico se estou tentando salvar uma chave
            if(keySave) {
                hexToMatriz(string, newKeySector);
            } else {
                // stringToChar(string, data);
                hexStringToBytes(string, data);
            }

            hexToByte(key, keySector);

            if (BlockConnection(block, keyType, keySector)) {
                keySave ? isWrite = WriteTag4Bytes(block, newKeySector) : isWrite = WriteTag4Bytes(block, data);
                if (isWrite) {
                    return jsonStatus("success");
                } else {
                    return jsonStatus("error");
                }
            } else {
                return jsonStatus("error");
            }

            SystemMode = 2;

        } else {
            messageProcess = "A função de escrita foi chamada, mas o modo escrita não está ativo.";
            return jsonStatus("error");
        }

    }

private:

    Adafruit_PN532 PN532;

    // Mensagem de Erro/Sucesso
    String messageProcess = "";

    // Todos os dados do cartão em json
    String jsonString;

    // Buffer para armazenar o UID retornado da Tag
    uint8_t uid[7] = { 0, 0, 0, 0, 0, 0, 0 };

    // Tamanho do UID da Tag (4 ou 7 bytes dependendo do tipo da Tag ISO14443A)
    uint8_t uidLength = 0;

    /**
     * Converte os dados da TAG de String para Char
     */
    void stringToChar(String string, uint8_t* data) {
        char charArray[string.length() + 1];
        string.toCharArray(charArray, string.length() + 1);
        for (int i = 0; i < string.length(); i++) {
            data[i] = (uint8_t)charArray[i];
        }
    }

    /**
     * Converte a key do setor de HEX para Byte
     */ 
    void hexToByte(String key, uint8_t* KeySector) {
        for (int i = 0; i < 6; i++) {
            char c1 = key.charAt(i * 2);
            char c2 = key.charAt(i * 2 + 1);
            int n1 = c1 >= 'A' ? c1 - 'A' + 10 : c1 - '0';
            int n2 = c2 >= 'A' ? c2 - 'A' + 10 : c2 - '0';
            KeySector[i] = (n1 << 4) | n2;
        }
    }

    /**
     * Converte a key do setor de HEX para Matriz uint8_t 
     */ 
    void hexToMatriz(String key, uint8_t* keyMatriz) {
        // Converte a string em uma matriz de bytes
        for (int i = 0; i < key.length(); i += 2) {
            // Extrai cada par de caracteres da string
            String byteString = key.substring(i, i + 2);
            // Converte o par de caracteres em um byte hexadecimal
            keyMatriz[i / 2] = strtoul(byteString.c_str(), NULL, 16);
        }
    }

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
     * Converte uma String HEX para Bytes
    */
    void hexStringToBytes(const String& hexString, uint8_t* bytes) {
        // Garante que o comprimento da string hexadecimal seja par
        if (hexString.length() % 2 != 0) {
            return; // Saia se o comprimento for ímpar
        }
        // Converte cada par de caracteres hexadecimais em um byte
        for (size_t i = 0; i < hexString.length(); i += 2) {
            String hexPair = hexString.substring(i, i + 2);
            bytes[i / 2] = strtol(hexPair.c_str(), NULL, 16);
        }
    }

    /**
     * Coloca os caracteres de uma String em maiúsculo.
    */
    String toUpperCase(const String &str) {
        String upperCase;
        for (size_t i = 0; i < str.length(); i++) {
            upperCase += char(toupper(str[i]));
        }
        return upperCase;
    }

    /**
     * Estrutura de um JSON de retorno.
    */
    String jsonStatus(String status) {
        return "{\"status\": \"" + String(status) + "\", \"message\": \"" + messageProcess + "\"}";
    }

    /**
     * Função para procurar e identificar uma tag ISO14443A usando o leitor PN532.
     * Aguarda uma Tag ISO14443A (Mifare, etc.) ser encontrada pela placa.
     * Quando encontrada armazenará o UID da Tag na variável uidLength
     * Se o UID tiver 4 bytes = (Mifare Classic) ou 7 bytes = (Mifare Ultralight)
     *
     * @return True se uma tag foi encontrada, False caso contrário.
     */
    bool TagConnection() {
        uint8_t connection;
        connection = PN532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if (connection) {
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
        bool isConnected = false;
        uint8_t connection;
        if (uidLength == 4) {
            connection = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, block, keyType, key);
            if (connection) {
                messageProcess = "O bloco foi autenticado.";
                isConnected = true;
            } else {
                messageProcess = "Nenhuma Tag lida ainda / A Tag lida não é a mesma que a Tag atual / A Chave de acesso está incorreta!";
            }

        }
        return isConnected;
    }

    /**
     * Função para escrever dados em um bloco de uma tag MIFARE Classic com 4 bytes de tamanho.
     * Verifica se o bloco informado não é reservado para a Key do Setor e, caso não seja, escreve os dados no bloco.
     * @param block Número do bloco onde os dados serão escritos.
     * @param data Array de 17 bytes contendo os dados a serem escritos na tag. O tamanho deve ser de 17 bytes, sendo o último byte reservado para o caractere nulo (terminador de string).
     */
    bool WriteTag4Bytes(int block, uint8_t data[17]) {
        uint8_t connection;
        connection = PN532.mifareclassic_WriteDataBlock(block, data);
        if (connection) {
            messageProcess = "Dados armazenados com sucesso!";
            return true;
        } else {
            messageProcess = "Houve um problema com a conexão do bloco.";
            return false;
        }
    }

    /**
     * Tenta autenticar um bloco de um setor a fim de encontrar a key correta
     * @param keys Vetor com as chaves recuperadas do SDCard
     * @param block Primeiro bloco do setor a ser checado.
     * @param keyType Tipo de autenticação 0 = Chave A, 1 = Chave B
     * @param data Ponteiro para o valor do bloco.
    */
    std::vector<String> AuthenticateBlock(std::vector<String> keys, uint8_t block, int keyType) {
        std::vector<String> foundKey(4);
        for (size_t i = 0; i < keys.size(); i++) {
            uint8_t success = TagConnection();
            if (success) {
                uint8_t tempKey[6];
                uint8_t tempData[16];
                hexToMatriz(keys[i], tempKey);
                success = PN532.mifareclassic_AuthenticateBlock(uid, uidLength, (block + 3), keyType, tempKey);
                if (success) {
                    success = PN532.mifareclassic_ReadDataBlock((block + 3), tempData);
                    if (success) {
                        foundKey[0] = byteToHexString(tempData, sizeof(tempData));
                        keyType == 0 ? foundKey[1] = keys[i] : foundKey[1] = toUpperCase(foundKey[0].substring(0, 12));
                        foundKey[2] = toUpperCase(foundKey[0].substring(12, 12 + 8));
                        foundKey[3] = toUpperCase(foundKey[0].substring(20, 20 + 12));
                        break;
                    }
                }
            }
        }
        return foundKey;
    }

    /**
     * Organiza as chaves encontradas dentro dos seus respectivos
     * lugares no JSON de saída.
     * @param keys Vetor com as chaves recuperadas do SDCard
     * @param sector Setor sendo percorrido no momento.
     * @param block Bloco do setor a ser validado.
    */
    std::vector<String> OrganizeAuthenticateData(std::vector<String> keys, int sector, uint8_t block) {
        std::vector<String> foundKey;
        std::vector<String> returnData(4);
        returnData[3] += "\"sector_" + String(sector) + "\":{";
        foundKey = AuthenticateBlock(keys, block, 0);
        if(foundKey[1] != "") {
            returnData[0] = foundKey[1];
            returnData[1] = foundKey[3];
            returnData[2] = "0";
            returnData[3] += "\"found_key\":\"A\",";
            returnData[3] += "\"key_a\":\"" + foundKey[1] + "\",";
            returnData[3] += "\"key_b\":\"" + foundKey[3] + "\",";
            returnData[3] += "\"key_acs\":\"" + foundKey[2] + "\",";
        } else {
            foundKey = AuthenticateBlock(keys, block, 1);
            returnData[0] = foundKey[1];
            returnData[1] = foundKey[3];
            returnData[2] = "1";
            foundKey[1] != "" ? returnData[3] += "\"found_key\":\"B\"," : returnData[3] += "\"found_key\":\"\",";
            returnData[3] += "\"key_a\":\"" + foundKey[1] + "\",";
            returnData[3] += "\"key_b\":\"" + foundKey[3] + "\",";
            returnData[3] += "\"key_acs\":\"" + foundKey[2] + "\",";
        }
        return returnData;
    }

};