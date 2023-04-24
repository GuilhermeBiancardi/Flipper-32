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

class InfraRedManager {

public:

    InfraRedManager() {}

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
    * Data: IrReceiver.decodedIRData.decodedRawData
    * Número de Bits: IrReceiver.decodedIRData.numberOfBits
    */
    bool ReceiveSinal() {
        if (IrReceiver.decode()) {

            // Mostra um pequeno sumário descritivo do sinal IR recebido
            // IrReceiver.printIRResultShort(&Serial);

            // Escreve um exemplo de código de envio do sinal lido
            // IrReceiver.printIRSendUsage(&Serial);

            // Se o sinal recebido for desconhecido
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
                Serial.println(F("Recebido sinal de um protocolo desconhecido ou não habilidatdo."));
                // Temos um protocolo desconhecido aqui, printo mais detalhes
                IrReceiver.printIRResultRawFormatted(&Serial, true);
            } else {

                // Salva o comando recebido pelo sinal infra vermelho
                // sCommand = IrReceiver.decodedIRData.command;

                // Salva o endereço recebido pelo sinal infra vermelho
                // sAddress = IrReceiver.decodedIRData.address;

                // Salva o rawData decodificado recebido pelo sinal infra vermelho
                sRawData = IrReceiver.decodedIRData.decodedRawData;

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
     * @param sRawData O RawData a ser enviado.
     * @param sRepeats O número de repetições do sinal a ser enviado.
     */
    void SendNecRawData(uint32_t sRawData, int_fast8_t sRepeats) {
        IrSender.sendNECRaw(sRawData, sRepeats);
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
            SDCard.FileWrite(path, String(sRawData).c_str());
            return true;
        } else {
            return false;
        }
    }

    const String GetRawData() {
        return String(sRawData);
    }

    int GetStatus() {
        return StatusIR;
    }

    void ResetIR() {
        StatusIR = 0;
        sRawData = 0;
        IrReceiver.resume();
    }

    void EmulateSinal(const char* path) {
        ReadData(path);
        SendNecRawData(sRawData, sRepeats);
    }

    void Loop() {

        if (StatusIR == 0) {
            if (ReceiveSinal()) {
                StatusIR = 1;
            }
        }

    }

private:

    // uint16_t sAddress;
    // uint16_t sCommand;
    
    int StatusIR = 0;
    uint32_t sRawData;
    uint16_t sRepeats = 5;

    void ReadData(const char* path) {
        String ReadData = SDCard.FileRead(path);
        sRawData = ReadData.toInt();
    }

};