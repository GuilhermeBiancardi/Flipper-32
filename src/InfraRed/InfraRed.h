#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

#define IR_SEND_PIN A4
#define IR_RECEIVE_PIN A7

class InfraRedManager {

public:

    InfraRedManager(uint8_t IRSendPin, uint8_t  IRReceivePin) : irsend(IRSendPin), irrecv(IRReceivePin) {}

    void Setup() {
        irsend.begin();
        irrecv.enableIRIn();
    }

    void Loop() {
        if (irrecv.decode(&results)) {
            uint16_t* raw_array = resultToRawArray(&results);
            uint16_t length = getCorrectedRawLength(&results);
            String value = uint64ToString(results.value, HEX);
            String address = Utils.Uint32_tToString(results.address);
            String command = Utils.Uint32_tToString(results.command);
            String bits = Utils.Uint16_tToString(results.bits);
            String state = Utils.Uint8_tToString(results.state, sizeof(results.state));
            String rawData = Utils.Uint16_tArrayToString(raw_array, length);
            String decodeType = decodeTypeToString(results.decode_type);
            irrecv.resume();
            delete[] raw_array;
            if (value != "FFFFFFFFFFFFFFFF") {
                jsonString = "{\"signal\": \"" + rawData + "\", \"value\": \"" + value + "\",";
                jsonString += "\"address\": \"" + address + "\", \"command\": \"" + command + "\",";
                jsonString += "\"decode\": \"" + decodeType + "\", \"bits\": \"" + bits + "\",";
                jsonString += "\"state\": \"" + state + "\", \"length\": \"" + length + "\"}";
            }
        }
        yield();
    }

    void SendSignal(String rawData, String size, String times) {
        uint16_t* raw_array = Utils.StringToUint16_tArray(rawData, size.toInt());
        uint16_t length = Utils.StringToUint16_t(size);
        for(size_t i = 0; i < times.toInt(); i++) {
            irsend.sendRaw(raw_array, length, irFrequency);
            delay(timeOut);
        }
        delete[] raw_array;
    }

    void SetJSON(String json) {
        jsonString = json;
    }

    String GetJSON() {
        return jsonString;
    }

private:

    IRsend irsend;
    IRrecv irrecv;
    decode_results results;

    String jsonString;

    // Buffer do que o esperado para que possamos lidar com mensagens IR muito grandes, ou seja, até 512 bits.
    uint16_t bufferSize = 1024;
    // Frequencia 
    uint16_t irFrequency = 38000;
    // Tempo limite é o número de milissegundos sem mais dados antes de considerarmos uma mensagem finalizada.
    uint8_t timeOut = 50;

    String decodeTypeToString(decode_type_t decodeType) {
        String result;
        switch (decodeType) {
            case UNKNOWN:
                result = "UNKNOWN";
                break;
            case UNUSED:
                result = "UNUSED";
                break;
            case RC5:
                result = "RC5";
                break;
            case RC6:
                result = "RC6";
                break;
            case NEC:
                result = "NEC";
                break;
            case SONY:
                result = "SONY";
                break;
            case PANASONIC:
                result = "PANASONIC";
                break;
            case JVC:
                result = "JVC";
                break;
            case SAMSUNG:
                result = "SAMSUNG";
                break;
            case WHYNTER:
                result = "WHYNTER";
                break;
            case AIWA_RC_T501:
                result = "AIWA_RC_T501";
                break;
            case LG:
                result = "LG";
                break;
            case SANYO:
                result = "SANYO";
                break;
            case MITSUBISHI:
                result = "MITSUBISHI";
                break;
            case DISH:
                result = "DISH";
                break;
            case SHARP:
                result = "SHARP";
                break;
            case COOLIX:
                result = "COOLIX";
                break;
            case DAIKIN:
                result = "DAIKIN";
                break;
            case DENON:
                result = "DENON";
                break;
            case KELVINATOR:
                result = "KELVINATOR";
                break;
            case SHERWOOD:
                result = "SHERWOOD";
                break;
            case MITSUBISHI_AC:
                result = "MITSUBISHI_AC";
                break;
            case RCMM:
                result = "RCMM";
                break;
            case SANYO_LC7461:
                result = "SANYO_LC7461";
                break;
            case RC5X:
                result = "RC5X";
                break;
            case GREE:
                result = "GREE";
                break;
            case PRONTO:
                result = "PRONTO";
                break;
            case NEC_LIKE:
                result = "NEC_LIKE";
                break;
            case ARGO:
                result = "ARGO";
                break;
            case TROTEC:
                result = "TROTEC";
                break;
            case NIKAI:
                result = "NIKAI";
                break;
            case RAW:
                result = "RAW";
                break;
            case GLOBALCACHE:
                result = "GLOBALCACHE";
                break;
            case TOSHIBA_AC:
                result = "TOSHIBA_AC";
                break;
            case FUJITSU_AC:
                result = "FUJITSU_AC";
                break;
            case MIDEA:
                result = "MIDEA";
                break;
            case MAGIQUEST:
                result = "MAGIQUEST";
                break;
            case LASERTAG:
                result = "LASERTAG";
                break;
            case CARRIER_AC:
                result = "CARRIER_AC";
                break;
            case HAIER_AC:
                result = "HAIER_AC";
                break;
            case MITSUBISHI2:
                result = "MITSUBISHI2";
                break;
            case HITACHI_AC:
                result = "HITACHI_AC";
                break;
            case HITACHI_AC1:
                result = "HITACHI_AC1";
                break;
            case HITACHI_AC2:
                result = "HITACHI_AC2";
                break;
            case GICABLE:
                result = "GICABLE";
                break;
            case HAIER_AC_YRW02:
                result = "HAIER_AC_YRW02";
                break;
            case WHIRLPOOL_AC:
                result = "WHIRLPOOL_AC";
                break;
            case SAMSUNG_AC:
                result = "SAMSUNG_AC";
                break;
            case LUTRON:
                result = "LUTRON";
                break;
            case ELECTRA_AC:
                result = "ELECTRA_AC";
                break;
            case PANASONIC_AC:
                result = "PANASONIC_AC";
                break;
            case PIONEER:
                result = "PIONEER";
                break;
            case LG2:
                result = "LG2";
                break;
            case MWM:
                result = "MWM";
                break;
            case DAIKIN2:
                result = "DAIKIN2";
                break;
            case VESTEL_AC:
                result = "VESTEL_AC";
                break;
            case TECO:
                result = "TECO";
                break;
            case SAMSUNG36:
                result = "SAMSUNG36";
                break;
            case TCL112AC:
                result = "TCL112AC";
                break;
            case LEGOPF:
                result = "LEGOPF";
                break;
            case MITSUBISHI_HEAVY_88:
                result = "MITSUBISHI_HEAVY_88";
                break;
            case MITSUBISHI_HEAVY_152:
                result = "MITSUBISHI_HEAVY_152";
                break;
            case DAIKIN216:
                result = "DAIKIN216";
                break;
            case SHARP_AC:
                result = "SHARP_AC";
                break;
            case GOODWEATHER:
                result = "GOODWEATHER";
                break;
            case INAX:
                result = "INAX";
                break;
            case DAIKIN160:
                result = "DAIKIN160";
                break;
            case NEOCLIMA:
                result = "NEOCLIMA";
                break;
            case DAIKIN176:
                result = "DAIKIN176";
                break;
            case DAIKIN128:
                result = "DAIKIN128";
                break;
            case AMCOR:
                result = "AMCOR";
                break;
            case DAIKIN152:
                result = "DAIKIN152";
                break;
            case MITSUBISHI136:
                result = "MITSUBISHI136";
                break;
            case MITSUBISHI112:
                result = "MITSUBISHI112";
                break;
            case HITACHI_AC424:
                result = "HITACHI_AC424";
                break;
            case SONY_38K:
                result = "SONY_38K";
                break;
            case EPSON:
                result = "EPSON";
                break;
            case SYMPHONY:
                result = "SYMPHONY";
                break;
            case HITACHI_AC3:
                result = "HITACHI_AC3";
                break;
            case DAIKIN64:
                result = "DAIKIN64";
                break;
            case AIRWELL:
                result = "AIRWELL";
                break;
            case DELONGHI_AC:
                result = "DELONGHI_AC";
                break;
            case DOSHISHA:
                result = "DOSHISHA";
                break;
            case MULTIBRACKETS:
                result = "MULTIBRACKETS";
                break;
            case CARRIER_AC40:
                result = "CARRIER_AC40";
                break;
            case CARRIER_AC64:
                result = "CARRIER_AC64";
                break;
            case HITACHI_AC344:
                result = "HITACHI_AC344";
                break;
            case CORONA_AC:
                result = "CORONA_AC";
                break;
            case MIDEA24:
                result = "MIDEA24";
                break;
            case ZEPEAL:
                result = "ZEPEAL";
                break;
            case SANYO_AC:
                result = "SANYO_AC";
                break;
            case VOLTAS:
                result = "VOLTAS";
                break;
            case METZ:
                result = "METZ";
                break;
            case TRANSCOLD:
                result = "TRANSCOLD";
                break;
            case TECHNIBEL_AC:
                result = "TECHNIBEL_AC";
                break;
            case MIRAGE:
                result = "MIRAGE";
                break;
            case ELITESCREENS:
                result = "ELITESCREENS";
                break;
            case PANASONIC_AC32:
                result = "PANASONIC_AC32";
                break;
            case MILESTAG2:
                result = "MILESTAG2";
                break;
            case ECOCLIM:
                result = "ECOCLIM";
                break;
            case XMP:
                result = "XMP";
                break;
            case TRUMA:
                result = "TRUMA";
                break;
            case HAIER_AC176:
                result = "HAIER_AC176";
                break;
            case TEKNOPOINT:
                result = "TEKNOPOINT";
                break;
            case KELON:
                result = "KELON";
                break;
            case TROTEC_3550:
                result = "TROTEC_3550";
                break;
            case SANYO_AC88:
                result = "SANYO_AC88";
                break;
            case BOSE:
                result = "BOSE";
                break;
            case ARRIS:
                result = "ARRIS";
                break;
            case RHOSS:
                result = "RHOSS";
                break;
            case AIRTON:
                result = "AIRTON";
                break;
            case COOLIX48:
                result = "COOLIX48";
                break;
            case HITACHI_AC264:
                result = "HITACHI_AC264";
                break;
            case KELON168:
                result = "KELON168";
                break;
            case HITACHI_AC296:
                result = "HITACHI_AC296";
                break;
            case DAIKIN200:
                result = "DAIKIN200";
                break;
            case HAIER_AC160:
                result = "HAIER_AC160";
                break;
            case CARRIER_AC128:
                result = "CARRIER_AC128";
                break;
            case TOTO:
                result = "TOTO";
                break;
            case CLIMABUTLER:
                result = "CLIMABUTLER";
                break;
            case TCL96AC:
                result = "TCL96AC";
                break;
            case BOSCH144:
                result = "BOSCH144";
                break;
            case SANYO_AC152:
                result = "SANYO_AC152";
                break;
            case DAIKIN312:
                result = "DAIKIN312";
                break;
            case GORENJE:
                result = "GORENJE";
                break;
            case WOWWEE:
                result = "WOWWEE";
                break;
            case CARRIER_AC84:
                result = "CARRIER_AC84";
                break;
            case YORK:
                result = "YORK";
                break;
            default:
                result = "INVALID";
        }
        return result;
    }

};