#include <IRremote.hpp>
#include <iostream>
#include <vector>

// DECODER padrão, inclue Apple e Onkyo
#define DECODE_NEC
// Habilita piscar o led do arduino ao receceber um sinal.
#define ENABLE_LED_FEEDBACK true
// Pino responsável pela leitura do sinal
#define IR_RECEIVE_PIN A7
// Pino responsável pelo envio do sinal
#define IR_SEND_PIN A4

// Definições Importantes para ESP32
#if !defined (FLASHEND)
// Dummy value for platforms where FLASHEND is not defined
#define FLASHEND 0xFFFF 
#endif
#if !defined (RAMEND)
// Dummy value for platforms where RAMEND is not defined
#define RAMEND 0xFFFF 
#endif
#if !defined (RAMSIZE)
// Dummy value for platforms where RAMSIZE is not defined
#define RAMSIZE 0xFFFF 
#endif

// Definições da Função STR
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

class InfraRed {

public:

    InfraRed() {}

    void Setup() {

        // Inicia a classe IR receiver
        IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

        // Inicia a classe IR sender
        IrSender.begin(IR_SEND_PIN);

        // Mostra no serial todos os protocolos suportados pelo DECODER escolhido
        // Serial.print(F("Pronto para receber sinais IR dos seguintes Protocolos: "));
        // printActiveIRProtocols(&Serial);
        // Serial.println(F("no pino " STR(IR_RECEIVE_PIN)));
    }

    /*
    * Checa se existe um sinal IR, se sim tenta decodifica-lo.
    * Resultados da decodificação fica armazenado na estrutura IrReceiver.decodedIRData.
    *
    * Comando IR fica armazenado em: IrReceiver.decodedIRData.command
    * Endereço do comando fica armazenado em: IrReceiver.decodedIRData.address
    * 32 bit Raw Data fica armazenado em: IrReceiver.decodedIRData.decodedRawData
    */
    bool ReceiveSinal() {
        if (IrReceiver.decode()) {

            // Mostra um pequeno sumário descritivo do sinal IR recebido
            IrReceiver.printIRResultShort(&Serial);

            // Escreve um exemplo de código de envio do sinal lido
            // IrReceiver.printIRSendUsage(&Serial);

            // Se o sinal recebido for desconhecido
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
                Serial.println(F("Recebido sinal de um protocolo desconhecido ou não habilidatdo."));
                // Temos um protocolo desconhecido aqui, printo mais detalhes
                IrReceiver.printIRResultRawFormatted(&Serial, true);
            } else {

                // Salva o comando recebido pelo sinal infra vermelho
                sCommand = IrReceiver.decodedIRData.command;
                // Salva o endereço recebido pelo sinal infra vermelho
                sAddress = IrReceiver.decodedIRData.address;
                // Salva o rawData decodificado recebido pelo sinal infra vermelho
                sRawData = IrReceiver.decodedIRData.decodedRawData;

                // Salva os valores do rawData
                for (int i = 0; i < IrReceiver.decodedIRData.rawDataPtr->rawlen; i++) {
                    sRawBuf[i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i];
                    StrRawBuf += String(sRawBuf[i]);
                    if (i < (IrReceiver.decodedIRData.rawDataPtr->rawlen - 1)) {
                        StrRawBuf += " ";
                    }
                }

            }

            // IMPORTANTE isso habilita o leitor a receber outro sinal IR
            IrReceiver.resume();

            return true;

        } else {
            return false;
        }
    }

    /**
     * Função que envia um comando utilizando o formato NEC do protocolo infravermelho.
     *
     * @param sAddress O endereço do dispositivo de destino (8 bits).
     * @param sCommand O comando a ser enviado (8 bits).
     * @param sRepeats O número de repetições do sinal a ser enviado.
     */
    void SendCommand(uint8_t sAddress, uint8_t sCommand, uint8_t sRepeats) {
        IrSender.sendNEC(sAddress, sCommand, sRepeats);
        IrReceiver.restartAfterSend();
    }

    /**
     * Função que envia dados brutos de infravermelho utilizando o formato RAW do protocolo infravermelho.
     *
     * @param rawData Um ponteiro constante para o array de dados brutos de infravermelho a ser enviado.
     * @note Certifique-se de que o tamanho do array de dados brutos está correto e corresponde ao formato RAW esperado pela biblioteca.
     * @note A forma como o tamanho do array é obtido pode não ser precisa, já que `rawData` é um ponteiro e não um array. Certifique-se de passar o tamanho do array corretamente ou usar uma forma alternativa de determinar o tamanho do array.
     */
    void SendRawData(const uint16_t* rawData) {
        IrSender.sendRaw(rawData, sizeof(rawData) / sizeof(rawData[0]), NEC_KHZ);
        IrReceiver.restartAfterSend();
    }

    /**
     * Função para salvar dados em um arquivo no cartão SD.
     *
     * @param path Caminho do arquivo a ser criado/sobrescrito no cartão SD.
     * @return true se o arquivo foi criado/sobrescrito com sucesso, false caso contrário.
     */
    bool SaveData(const char* path) {
        if (SDCard.FileCreate(path)) {
            // Escreve os dados no arquivo
            SDCard.FileWrite(path, StrRawBuf.c_str());
            return true;
        } else {
            return false;
        }
    }

    const String GetRawData() {
        return StrRawBuf;
    }

    int GetStatus() {
        return StatusIR;
    }

    void ResetIR() {
        StatusIR = 0;
        StrRawBuf = "";
        IrReceiver.resume();
    }

    void EmulateSinal(const char* path) {
        ReadData(path);
        const uint16_t* sRawBufInMemory = reinterpret_cast<const uint16_t*>(sRawBuf);
        SendRawData(sRawBufInMemory);
    }

    void Loop() {

        if (StatusIR == 0) {
            if (ReceiveSinal()) {
                StatusIR = 1;
            }
        }

        // Exemplo de uso com o SDCard
        // if (StatusIR == 0) {
        //     if (ReceiveSinal()) {
        //         StatusIR = 1;
        //     }
        // } else if (StatusIR == 1) {
        //     if (SaveData("DataCapture.txt")) {
        //         StatusIR = 2;
        //     }
        //     StatusIR = 2;
        // } else if (StatusIR == 2) {
        //     ReadData("DataCapture.txt");
        //     StatusIR = 3;
        // } else if (StatusIR == 3) {
        //     delay(2000);
        //     const uint16_t* sRawBufInMemory = reinterpret_cast<const uint16_t*>(sRawBuf);
        //     SendRawData(sRawBufInMemory);
        // }

    }

private:

    int StatusIR = 0;
    unsigned int sRawBuf[RAW_BUFFER_LENGTH];
    String StrRawBuf;

    uint32_t sRawData;
    uint16_t sAddress;
    uint16_t sCommand;
    uint16_t sRepeats = 0;

    void ExplodeString(const char* s, const char* delim, std::vector<unsigned int>& result) {
        const char* start = s;
        const char* end = s;
        int delimLen = strlen(delim);
        while (*end) {
            if (strncmp(end, delim, delimLen) == 0) {
                if (end > start) {
                    unsigned int num = 0;
                    sscanf(start, "%u", &num);
                    result.push_back(num);
                }
                start = end + delimLen;
            }
            ++end;
        }
        if (end > start) {
            unsigned int num = 0;
            sscanf(start, "%u", &num);
            result.push_back(num);
        }
    }

    void ReadData(const char* path) {
        std::vector<unsigned int> Dados;
        ExplodeString(SDCard.FileRead(path).c_str(), " ", Dados);
        std::copy(Dados.begin(), Dados.end(), sRawBuf);
    }

};