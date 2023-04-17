#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "Pages/index.h"
#include "Pages/load.h"

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

EEPROMManager InternalMemory(4096);

AsyncWebServer server(80);

boolean connectWifi();

void WebServiceSetup() {

	hardware = InternalMemory.ReadStr(0, 100);
	wifissid = InternalMemory.ReadStr(100, 100);
	password = InternalMemory.ReadStr(200, 100);
	passhard = InternalMemory.ReadStr(300, 100);

    if(hardware != NULL) {
		ssid = hardware;
    }

    if(passhard != NULL) {
		pass = passhard;
    }

	if(wifissid != NULL && password != NULL) {
        connectWifi();
	}

    Serial.println(ssid);
    Serial.println(pass);
	
    WiFi.softAP(ssid.c_str(), pass.c_str());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    server.on("/RESET", HTTP_GET, [](AsyncWebServerRequest *request){
        InternalMemory.Clear(4096);
        request->send_P(200, "text/html", load_html);
    });

    server.on("/GET", HTTP_GET, [](AsyncWebServerRequest *request) {
        
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

        request->send_P(200, "text/html", load_html);

    });

    server.on("/RESTART", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", load_html);
    });

    server.on("/hardware", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", String(hardware).c_str());
    });

    server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", String(wifissid).c_str());
    });

    server.on("/password", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", String(password).c_str());
    });

    server.on("/passhard", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", String(passhard).c_str());
    });

    server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", WiFi.localIP().toString().c_str());
    });

    server.on("/reload", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", "<script>window.location = '/';</script>");
        ESP.restart();
    });
    
    server.begin();

}

void WebServiceLoop() {
	Serial.println("AP IP:");
	Serial.println(WiFi.softAPIP());
	Serial.println("Local IP:");
    Serial.println(WiFi.localIP());

	Serial.println(hardware);
	Serial.println(wifissid);
	Serial.println(password);
	delay(500);
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