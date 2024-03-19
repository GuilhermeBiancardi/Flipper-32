#include <WiFi.h>
#include <ESPAsyncWebServer.h>

String ssid = "Flipper32";
String pass = "123456789";

const char* PARAM_INPUT_1 = "hardware";
const char* PARAM_INPUT_2 = "wifissid";
const char* PARAM_INPUT_3 = "password";
const char* PARAM_INPUT_4 = "passhard";

String hardware = "";
String wifissid = "";
String password = "";
String passhard = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/socket");

boolean connectWifi();

void handleWebSocket(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
    } else if (type == WS_EVT_DATA) {
        Serial.print("Received data from client: ");
        Serial.write(data, len);
    }
}

void WebServiceSetup() {

    hardware = InternalMemory.ReadStr(0, 100);
    wifissid = InternalMemory.ReadStr(100, 100);
    password = InternalMemory.ReadStr(200, 100);
    passhard = InternalMemory.ReadStr(300, 100);

    if (hardware != NULL) {
        ssid = hardware;
    }

    if (passhard != NULL) {
        pass = passhard;
    }

    if (wifissid != NULL && password != NULL) {
        connectWifi();
    }

    WiFi.softAP(ssid.c_str(), pass.c_str());

    /**
     * Toda requisição não registrada passa por aqui, se o arquivo requisitado
     * exisir no SD Card ele é carregado.
    */
    server.onNotFound(
        [](AsyncWebServerRequest* request) {

            // Obtém o caminho da URL solicitada
            String path = request->url();

            int lastIndex = path.lastIndexOf(".");
            String extension = path.substring(lastIndex + 1);

            Serial.print(path);
            Serial.print(" - ");
            Serial.println(extension);

            // Monta o caminho completo do arquivo no cartão SD
            String filename = "System/App" + path;

            // Checa se o caminho requisitado existe no SDCard
            if (SDCard.CheckExists(filename.c_str())) {
                Serial.print("Carregando: ");
                Serial.println(filename);

                AsyncWebServerResponse* response = request->beginResponse(SD, "/" + filename, "");
                
                // Verifica se o conteúdo é HTML
                if(extension != "html") {
                    // Se não for adiciona o conteúdo da requisição no cache por 1 ano
                    response->addHeader("Cache-Control", "max-age=31536000");
                }

                if(extension != "map") {
                    request->send(response);
                } else {
                    request->send_P(404, "text/plain", "File not Found!");
                }


            } else {
                request->send_P(404, "text/plain", "File not Found!");
            }

        }
    );

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String filename = "System/App/cache.html";
            String html;

            html = SDCard.FileRead(filename.c_str());
            request->send_P(200, "text/html", html.c_str());

        }
    );

    // // --------- IR -----------

    // server.on("/IR", HTTP_GET,
    //     [](AsyncWebServerRequest* request) {
    //         request->send_P(200, "text/html", ir_html);
    //         SystemMode = 1;
    //     }
    // );

    // server.on("/IRSAVE", HTTP_GET,
    //     [](AsyncWebServerRequest* request) {

    //         String FileName;
    //         String IRData;

    //         if (request->hasParam("ir_nome")) {
    //             FileName = request->getParam("ir_nome")->value();
    //         }

    //         if (request->hasParam("ir_code")) {
    //             IRData = request->getParam("ir_code")->value();
    //         }

    //         IR.SaveData(FileName.c_str());

    //         IR.ResetIR();

    //         request->send_P(200, "text/plain", "success");
    //         // request->send_P(200, "text/html", ir_html);

    //     }
    // );

    // server.on("/IRGET", HTTP_GET,
    //     [](AsyncWebServerRequest* request) {

    //         String FileName;

    //         if (request->hasParam("ir_nome")) {
    //             FileName = request->getParam("ir_nome")->value();
    //         }

    //         IR.EmulateSinal(FileName.c_str());

    //         request->send_P(200, "text/plain", "success");

    //     }
    // );

    // server.on("/IRDATA", HTTP_GET,
    //     [](AsyncWebServerRequest* request) {
    //         IR.ResetIR();
    //         request->send_P(200, "text/plain", String(IR.GetRawData()).c_str());
    //     }
    // );

    // --------- CONFIG -----------

    server.on("/CONFIG_GET_DATA", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            String IP = WiFi.localIP().toString();
            String json = "{\"IP\": \"" + IP + "\", \"SSID\": \"" + String(wifissid.c_str()) + "\", \"PASS\": \"" + String(password.c_str()) + "\",";
            json += "\"HSSID\": \"" + String(hardware.c_str()) + "\", \"HPASS\": \"" + String(passhard.c_str()) + "\"}";
            request->send_P(200, "text/plain", json.c_str());
        }
    );
    
    server.on("/CONFIG_RESET", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            ESP.restart();
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/CONFIG_SAVE_INTERNAL_WIFI", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String ssid = "";
            String pass = "";
            String json = "{\"status\": \"success\", \"message\": \"Os dados foram atualizados.\"}";

            if (request->hasParam("HSSID")) {
                ssid = request->getParam("HSSID")->value().c_str();
            }

            if (request->hasParam("HPASS")) {
                pass = request->getParam("HPASS")->value().c_str();
            }

            hardware = ssid;
            passhard = pass;
            InternalMemory.WriteStr(0, ssid);
            InternalMemory.WriteStr(300, pass);

            request->send_P(200, "text/plain", json.c_str());

        }
    );

    server.on("/CONFIG_SAVE_EXTERNAL_WIFI", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String ssid = "";
            String pass = "";
            String json = "{\"status\": \"success\", \"message\": \"Os dados foram atualizados.\"}";

            if (request->hasParam("SSID")) {
                ssid = request->getParam("SSID")->value();
            }

            if (request->hasParam("PASS")) {
                pass = request->getParam("PASS")->value();
            }

            wifissid = ssid;
            password = pass;
            InternalMemory.WriteStr(100, ssid);
            InternalMemory.WriteStr(200, pass);

            request->send_P(200, "text/plain", json.c_str());

        }
    );

    // --------- GERAL -----------

    server.on("/SYSTEM_OFF", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            SystemMode = 0;
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/FILE_OPEN", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String path = "System/";

            if (request->hasParam("path")) {
                path += request->getParam("path")->value();
            }

            String json = SDCard.FileRead(path.c_str());
            request->send_P(200, "text/plain", json.c_str());
        }
    );

    server.on("/FILE_SAVE", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String path = "System/";
            String json = "";

            if (request->hasParam("path")) {
                path += request->getParam("path")->value();
            }

            if (request->hasParam("json")) {
                json = request->getParam("json")->value();
            }

            if(SDCard.FileWrite(path.c_str(), json.c_str())) {
                request->send_P(200, "text/plain", "{\"status\": \"success\", \"message\": \"Dados salvos com sucesso.\"}");
            } else {
                request->send_P(200, "text/plain", "{\"status\": \"error\", \"message\": \"Os dados não foram salvos.\"}");
            }

        }
    );

    server.on("/DELETE_FILE", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";
            if (request->hasParam("path")) {
                path += request->getParam("path")->value();
            }

            if(SDCard.Delete(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    server.on("/CREATE_FILE", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";

            if (request->hasParam("path")) {
                path += request->getParam("path")->value() + ".json";
            }

            if(SDCard.FileCreate(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    server.on("/CREATE_FOLDER", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";

            if (request->hasParam("path")) {
                path += request->getParam("path")->value();
            }

            if(SDCard.FolderCreate(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    // ---------- NFC ------------

    server.on("/NFC_LIST_DIR", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            String json = SDCard.ListDirectory("/System/NFC");
            request->send_P(200, "text/plain", json.c_str());
        }
    );

    server.on("/NFC_READ_ON", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            SystemMode = 2;
            PN532.SetJSON("");
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/NFC_WRITE_ON", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            
            SystemMode = 3;

            int block;
            int type;
            String key = "";
            String data = "";

            if (request->hasParam("block")) {
                block = request->getParam("block")->value().toInt();
            }

            if (request->hasParam("type")) {
                type = request->getParam("type")->value().toInt();
            }

            if (request->hasParam("key")) {
                key = request->getParam("key")->value();
            }

            if (request->hasParam("data")) {
                data = request->getParam("data")->value();
            }

            String response = PN532.StartWriteData(data, block, type, key);

            request->send_P(200, "text/plain", response.c_str());
        }
    );

    ws.onEvent(handleWebSocket);
    server.addHandler(&ws);
    server.begin();

}

void WebServiceLoop() {

    if(SystemMode == 2) {
        String json = PN532.GetJSON();
        if(json != "") {
            ws.textAll(json);
            PN532.SetJSON("");
        }
    }

}

boolean connectWifi() {

    boolean state = true;
    int i = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifissid.c_str(), password.c_str());

    Serial.println("Conectando no WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (i > 20) {
            state = false;
            break;
        }
        i++;
    }
    return state;

}