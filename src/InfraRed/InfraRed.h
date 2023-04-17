#include <IRremote.hpp>

// DECODER padrão, inclue Apple e Onkyo
#define DECODE_NEC
// Habilita piscar o led do arduino ao receceber um sinal.
#define ENABLE_LED_FEEDBACK true
// Pino responsável pela leitura do sinal
#define IR_RECEIVE_PIN A7
// Pino responsável pelo envio do sinal
#define IR_SEND_PIN A4

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
    void ReceiveIRSinal() {
        if (IrReceiver.decode()) {

            // Mostra um pequeno sumário descritivo do sinal IR recebido
            IrReceiver.printIRResultShort(&Serial);
            IrReceiver.printIRSendUsage(&Serial);

            // Se o sinal recebido for desconhecido
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
                Serial.println(F("Recebido sinal de um protocolo desconhecido ou não habilidatdo."));
                // Temos um protocolo desconhecido aqui, printo mais detalhes
                IrReceiver.printIRResultRawFormatted(&Serial, true);
            }
            Serial.println();

            // IMPORTANTE isso habilita o leitor a receber outro sinal IR
            IrReceiver.resume();

        }
    }

    /**
     * Envia um sinal IR
    */
    void SendIRSinal(uint8_t sAddress, uint8_t sCommand, uint8_t sRepeats) {
        IrSender.sendNEC(sAddress, sCommand, sRepeats);
        IrReceiver.restartAfterSend();
    }

    void Loop() {

        ReceiveIRSinal();

        // Finalmente faço checagens referente ao sinal recebido
        if (IrReceiver.decodedIRData.command == 0x10) {
            Serial.println("Sinal: 10");
            SendIRSinal(sAddress, sCommand, sRepeats);
        }

        delay(100);

    }

private:

    uint16_t sAddress = 0x04;
    uint16_t sCommand = 0x08;
    uint32_t sRawData = 0xF708FB04;
    uint16_t sRepeats = 5;

};