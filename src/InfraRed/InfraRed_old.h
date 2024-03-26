// Porta responsável pelo LED.
#define LED_BUILTIN 4

#if !defined(RAW_BUFFER_LENGTH)
#  if RAMEND <= 0x4FF || RAMSIZE < 0x4FF
#define RAW_BUFFER_LENGTH  180  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#  elif RAMEND <= 0x8FF || RAMSIZE < 0x8FF
#define RAW_BUFFER_LENGTH  600  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#  else
#define RAW_BUFFER_LENGTH  750  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#  endif
#endif

#include <IRremote.hpp>
#include <iostream>

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

    String GetJSON() {
        ResetIR();
        return jsonString;
    }

    void SetJSON(String json) {
        jsonString = json;
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

            auto tStartMillis = millis();
            IrReceiver.stopTimer();

            // Mostra um pequeno sumário descritivo do sinal IR recebido
            IrReceiver.printIRResultShort(&Serial);

            // Escreve um exemplo de código de envio do sinal lido
            IrReceiver.printIRSendUsage(&Serial);

            // Print ticks, this is faster :-)
            IrReceiver.printIRResultRawFormatted(&Serial);

            if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
                Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));
            }

            // Se o sinal recebido for desconhecido
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
                Serial.println(F("Recebido sinal de um protocolo desconhecido ou não habilidatdo."));

                uint8_t tNumberOfBits = (IrReceiver.decodedIRData.rawDataPtr->rawlen + 1) / 2;
                uint_fast8_t tPrintedStringLength = Serial.print(tNumberOfBits);
                Serial.print(F(" bit "));

                if (IrReceiver.decodedIRData.decodedRawData != 0) {
                    if (tNumberOfBits < 10) {
                        Serial.print('0');
                        tPrintedStringLength++;
                    }
                    Serial.print('x');
                    tPrintedStringLength += Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX) + 1;
                } else {
                    tPrintedStringLength += Serial.print(IrReceiver.getTotalDurationOfRawData());
                    Serial.print(F(" \xE4s")); // \xE4 is micro symbol
                }

                // Temos um protocolo desconhecido aqui, printo mais detalhes
                // IrReceiver.printIRResultRawFormatted(&Serial, true);

                jsonString = "{\"signal\": \"UNKNOWN\"}";
            } else {

                // Salva o comando recebido pelo sinal infra vermelho
                // sCommand = IrReceiver.decodedIRData.command;

                // Salva o endereço recebido pelo sinal infra vermelho
                // sAddress = IrReceiver.decodedIRData.address;

                // Salva o rawData decodificado recebido pelo sinal infra vermelho
                sRawData = IrReceiver.decodedIRData.decodedRawData;

                // Salva o index do protocolo recebido
                sProtocolIndex = IrReceiver.decodedIRData.protocol;
                // Retorna o nome do protocolo
                //getProtocolString(sProtocolIndex);

                // Salva o endereço do protocolo
                sProtocolAddress = IrReceiver.decodedIRData.address;

                uint_fast8_t tAddressStringLength = Serial.println(IrReceiver.decodedIRData.decodedRawDataArray[0], HEX);

                uint16_t tCommand = IrReceiver.decodedIRData.command;
                Serial.println(tCommand);

                String sRawDataString = String(sRawData);
                jsonString = "{\"signal\": \"" + sRawDataString + "\"}";

            }

            while ((millis() - tStartMillis) < 5);

            // Restore IR timer. millis() - tStartMillis to compensate for stop of receiver. This enables a correct gap measurement.
            IrReceiver.restartTimerWithTicksToAdd((millis() - tStartMillis) * (MICROS_IN_ONE_MILLI / MICROS_PER_TICK));

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
    
    String jsonString;

    uint16_t sProtocolIndex = 4711;
    uint16_t sProtocolAddress = 4711;

    uint32_t sRawData;
    uint16_t sRepeats = 5;

    void ReadData(const char* path) {
        String ReadData = SDCard.FileRead(path);
        sRawData = ReadData.toInt();
    }

};