#include <WiFi.h>
#include <ESPAsyncWebServer.h>

String ssid = "Flipper32";
String pass = "123456789";

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

    server.on("/SYSTEM_OFF", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            SystemMode = 0;
            request->send_P(200, "text/plain", "ok");
        }
    );

    // // --------- IR -----------

    server.on("/IR_READ_ON", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            SystemMode = 1;
            IR.SetJSON("");
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/IR_EMULATE", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            String rawData = "", size = "", times = "";

            if (request->hasParam("rawData", true)) {
                rawData = request->getParam("rawData", true)->value();
            }

            if (request->hasParam("size", true)) {
                size = request->getParam("size", true)->value();
            }

            if (request->hasParam("times", true)) {
                times = request->getParam("times", true)->value();
            }

            IR.SendSignal(rawData, size, times);

            request->send_P(200, "text/plain", "ok");

        }
    );

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

    server.on("/CONFIG_GET_DATA", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            String IP = WiFi.localIP().toString();
            String json = "{\"IP\": \"" + IP + "\", \"SSID\": \"" + String(wifissid.c_str()) + "\", \"PASS\": \"" + String(password.c_str()) + "\",";
            json += "\"HSSID\": \"" + String(hardware.c_str()) + "\", \"HPASS\": \"" + String(passhard.c_str()) + "\"}";
            request->send_P(200, "text/plain", json.c_str());
        }
    );
    
    server.on("/CONFIG_RESET", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            ESP.restart();
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/CONFIG_SAVE_INTERNAL_WIFI", HTTP_POST,
        [](AsyncWebServerRequest* request) {

            String ssid = "";
            String pass = "";
            String json = "{\"status\": \"success\", \"message\": \"Os dados foram atualizados.\"}";

            if (request->hasParam("HSSID", true)) {
                ssid = request->getParam("HSSID", true)->value();
            }

            if (request->hasParam("HPASS", true)) {
                pass = request->getParam("HPASS", true)->value();
            }

            hardware = ssid;
            passhard = pass;
            InternalMemory.WriteStr(0, ssid);
            InternalMemory.WriteStr(300, pass);

            request->send_P(200, "text/plain", json.c_str());

        }
    );

    server.on("/CONFIG_SAVE_EXTERNAL_WIFI", HTTP_POST,
        [](AsyncWebServerRequest* request) {

            String ssid = "";
            String pass = "";
            String json = "{\"status\": \"success\", \"message\": \"Os dados foram atualizados.\"}";

            if (request->hasParam("SSID", true)) {
                ssid = request->getParam("SSID", true)->value();
            }

            if (request->hasParam("PASS", true)) {
                pass = request->getParam("PASS", true)->value();
            }

            wifissid = ssid;
            password = pass;
            InternalMemory.WriteStr(100, ssid);
            InternalMemory.WriteStr(200, pass);

            request->send_P(200, "text/plain", json.c_str());

        }
    );

    // --------- SDCard -----------

    server.on("/LIST_DIR", HTTP_POST,
        [](AsyncWebServerRequest* request) {

            String path = "/System/";

            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value();
            }

            String json = SDCard.ListDirectory(path.c_str());
            request->send_P(200, "text/plain", json.c_str());
        }
    );

    server.on("/FILE_OPEN", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";

            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value();
            }

            String data = SDCard.FileRead(path.c_str());
            request->send_P(200, "text/plain", data.c_str());
        }
    );

    server.on("/FILE_SAVE", HTTP_POST,
        [](AsyncWebServerRequest* request) {

            String path = "System/";
            String data = "";

            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value();
            }

            if (request->hasParam("data", true)) {
                data = request->getParam("data", true)->value();
            }

            if(SDCard.FileWrite(path.c_str(), data.c_str())) {
                request->send_P(200, "text/plain", "{\"status\": \"success\", \"message\": \"Dados salvos com sucesso.\"}");
            } else {
                request->send_P(200, "text/plain", "{\"status\": \"error\", \"message\": \"Os dados não foram salvos.\"}");
            }

        }
    );

    server.on("/DELETE_FILE", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";
            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value();
            }

            if(SDCard.Delete(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    server.on("/CREATE_FILE", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";

            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value() + ".json";
            }

            if(SDCard.FileCreate(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    server.on("/CREATE_FOLDER", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            String path = "System/";

            if (request->hasParam("path", true)) {
                path += request->getParam("path", true)->value();
            }

            if(SDCard.FolderCreate(path.c_str())) {
                request->send_P(200, "text/plain", "ok");
            } else {
                request->send_P(200, "text/plain", "er");
            }

        }
    );

    // ---------- NFC ------------

    server.on("/NFC_READ_ON", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            SystemMode = 2;
            PN532.SetJSON("");
            request->send_P(200, "text/plain", "ok");
        }
    );

    server.on("/NFC_WRITE_ON", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            SystemMode = 3;

            int block;
            int type;
            String key = "";
            String data = "";

            if (request->hasParam("block", true)) {
                block = request->getParam("block", true)->value().toInt();
            }

            if (request->hasParam("type", true)) {
                type = request->getParam("type", true)->value().toInt();
            }

            if (request->hasParam("key", true)) {
                key = request->getParam("key", true)->value();
            }

            if (request->hasParam("data", true)) {
                data = request->getParam("data", true)->value();
            }

            String response = PN532.StartWriteData(data, block, type, key);

            request->send_P(200, "text/plain", response.c_str());
        }
    );

    server.on("/NFC_WRITE_KEY_ON", HTTP_POST,
        [](AsyncWebServerRequest* request) {
            
            SystemMode = 3;

            int block;
            String key_a, key_b, new_key, response;

            if (request->hasParam("block", true)) {
                block = request->getParam("block", true)->value().toInt();
            }

            if (request->hasParam("key_a", true)) {
                key_a = request->getParam("key_a", true)->value();
            }

            if (request->hasParam("key_b", true)) {
                key_b = request->getParam("key_b", true)->value();
            }

            if (request->hasParam("new_key", true)) {
                new_key = request->getParam("new_key", true)->value();
            }

            if(key_a != "000000000000") {
                response = PN532.StartWriteData(new_key, block, 0, key_a, true);
            } else {
                response = PN532.StartWriteData(new_key, block, 1, key_b, true);
            }

            request->send_P(200, "text/plain", response.c_str());
        }
    );

    ws.onEvent(handleWebSocket);
    server.addHandler(&ws);
    server.begin();

}

void WebServiceLoop() {

    if(SystemMode == 1) {
        String json = IR.GetJSON();
        if(json != "") {
            ws.textAll(json);
            IR.SetJSON("");
        }
    }

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

// JSON Exemple
// server.on("/state", HTTP_GET, []() {
// DynamicJsonDocument root(1024);
// root["mode"] = acState.operation;
// root["fan"] = acState.fan;
// root["temp"] = acState.temperature;
// root["power"] = acState.powerStatus;
// String output;
// serializeJson(root, output);
// server.send(200, "text/plain", output);
// });
// DeserializationError error = deserializeJson(root, server.arg("plain"));