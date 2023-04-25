#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "Pages/index.h"
#include "Pages/load_config.h"
#include "Pages/infra_vermelho.h"
#include "Pages/config.h"

String ssid = "FliperMCU";
String pass = "12345678";

const char* PARAM_INPUT_1 = "hardware";
const char* PARAM_INPUT_2 = "wifissid";
const char* PARAM_INPUT_3 = "password";
const char* PARAM_INPUT_4 = "passhard";

String hardware = "";
String wifissid = "";
String password = "";
String passhard = "";

AsyncWebServer server(80);

boolean connectWifi();

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

    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     request->send_P(200, "text/html", index_html);
    // });

    /**
     * Toda requisição não registrada passa por aqui, se o arquivo requisitado
     * exisir no SD Card ele é carregado.
    */
    server.onNotFound(
        [](AsyncWebServerRequest* request) {

            // Obtém o caminho da URL solicitada
            String path = request->url();

            Serial.println(path);

            // Monta o caminho completo do arquivo no cartão SD
            String filename = "System/App" + path;

            // Checa se o caminho requisitado existe no SDCard
            if (SDCard.CheckExists(filename.c_str())) {
                Serial.print("Carregando: ");
                Serial.println(filename);

                AsyncWebServerResponse* response = request->beginResponse(SD, "/" + filename, "");
                response->addHeader("Cache-Control", "max-age=3600");
                request->send(response);
                
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

    server.on("/RESET", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            InternalMemory.Clear(4096);
            request->send_P(200, "text/html", load_config_html);
        }
    );

    server.on("/GET", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            // GET input value on <ESP_IP>/GET?input=<inputValue>
            if (request->hasParam(PARAM_INPUT_1)) {
                hardware = request->getParam(PARAM_INPUT_1)->value();
            }

            if (request->hasParam(PARAM_INPUT_2)) {
                wifissid = request->getParam(PARAM_INPUT_2)->value();
            }

            if (request->hasParam(PARAM_INPUT_3)) {
                password = request->getParam(PARAM_INPUT_3)->value();
            }

            if (request->hasParam(PARAM_INPUT_4)) {
                passhard = request->getParam(PARAM_INPUT_4)->value();
            }

            InternalMemory.WriteStr(0, hardware);
            InternalMemory.WriteStr(100, wifissid);
            InternalMemory.WriteStr(200, password);
            InternalMemory.WriteStr(300, passhard);

            request->send_P(200, "text/html", load_config_html);

        }
    );

    server.on("/RESTART", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/html", load_config_html);
        }
    );

    server.on("/hardware", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/plain", String(hardware).c_str());
        }
    );

    server.on("/ssid", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/plain", String(wifissid).c_str());
        }
    );

    server.on("/password", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/plain", String(password).c_str());
        }
    );

    server.on("/passhard", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/plain", String(passhard).c_str());
        }
    );

    server.on("/ip", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/plain", WiFi.localIP().toString().c_str());
        }
    );

    server.on("/reload", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/html", "<script>window.location = '/';</script>");
            ESP.restart();
        }
    );

    // --------- IR -----------

    server.on("/IR", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/html", ir_html);
            SystemMode = 1;
        }
    );

    server.on("/IRSAVE", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String FileName;
            String IRData;

            if (request->hasParam("ir_nome")) {
                FileName = request->getParam("ir_nome")->value();
            }

            if (request->hasParam("ir_code")) {
                IRData = request->getParam("ir_code")->value();
            }

            IR.SaveData(FileName.c_str());

            IR.ResetIR();

            request->send_P(200, "text/plain", "success");
            // request->send_P(200, "text/html", ir_html);

        }
    );

    server.on("/IRGET", HTTP_GET,
        [](AsyncWebServerRequest* request) {

            String FileName;

            if (request->hasParam("ir_nome")) {
                FileName = request->getParam("ir_nome")->value();
            }

            IR.EmulateSinal(FileName.c_str());

            request->send_P(200, "text/plain", "success");

        }
    );

    server.on("/IRDATA", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            IR.ResetIR();
            request->send_P(200, "text/plain", String(IR.GetRawData()).c_str());
        }
    );

    // --------- CONFIG -----------

    server.on("/CONFIG", HTTP_GET,
        [](AsyncWebServerRequest* request) {
            request->send_P(200, "text/html", config_html);
        }
    );

    server.begin();

}

void WebServiceLoop() {

    // Serial.println("AP IP:");
    // Serial.println(WiFi.softAPIP());
    // Serial.println("Local IP:");
    // Serial.println(WiFi.localIP());

    // Serial.println(hardware);
    // Serial.println(wifissid);
    // Serial.println(password);
    // delay(500);

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