#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <LiquidCrystal.h>
#include "index.h"
#include "setup.h"

#define ExtLED 3  // Use a valid GPIO for ESP32-C3
#define BootLED 8

MDNSResponder mdns;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
const char* redirectIP = "172.217.28.1";
int beep;

Preferences preferences;

struct settings {
  char ssid[30];
  char password[30];
  char dns[30];
} user_wifi = {};

void handleSetup() {
  server.sendHeader("Location", String("http://") + redirectIP, true);
  server.send(302, "text/plain", "Redirecting to the setup page...");
}

void setup() {
  Serial.begin(9600);

  preferences.begin("wifi_config", false);
  preferences.getBytes("ssid", user_wifi.ssid, sizeof(user_wifi.ssid));
  preferences.getBytes("password", user_wifi.password, sizeof(user_wifi.password));

  Serial.println("Booting...");
  if (preferences.getString("ssid", user_wifi.ssid) != NULL) {
    Serial.print("Connecting to: ");
    Serial.println(preferences.getString("ssid", user_wifi.ssid));
  }
  pinMode(BootLED, OUTPUT);
  delay(100);
  digitalWrite(BootLED, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  if (preferences.getString("ssid", user_wifi.ssid) != NULL) {
    int bootcount;
    while (bootcount < 6) {
      if (bootcount < 5) {
        Serial.print("...");
        delay(2000);
        bootcount += 1;
      } else {
        Serial.println("...");
        delay(1000);
        bootcount += 1;
      }
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    if (preferences.getString("ssid", user_wifi.ssid) != NULL){
      Serial.print("Failed to connect to: ");
      Serial.println(preferences.getString("ssid", user_wifi.ssid));
    }
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("SmartLink Outlet");
    dnsServer.start(DNS_PORT, "*", apIP);
    server.onNotFound(handleSetup);
    server.on("/", handlePortal);
    server.begin();
    while (beep < 10) {
      pinMode(BootLED, OUTPUT);
      if (digitalRead(BootLED) == LOW) {
        digitalWrite(BootLED, HIGH);
        delay(400);
        beep += 1;
      } else if (digitalRead(BootLED) == HIGH) {
        digitalWrite(BootLED, LOW);
        delay(400);
        beep += 1;
      }
    }
    digitalWrite(BootLED, LOW);

    Serial.println("Please use WiFi \"SmartLink Outlet\" to setup device");
    server.on("/scan", HTTP_GET, handleWiFiScan);
  } else {
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    delay(5000);
    if (preferences.getString("DNS", user_wifi.dns) != NULL) {
      if (!MDNS.begin(preferences.getString("DNS", user_wifi.dns))) { // Numele pe care îl vei folosi pentru acces
      Serial.println("Eroare la inițializarea mDNS");
      return;
      }
      Serial.print("mDNS inițializat cu numele ");
      Serial.print("http://");
      Serial.print(preferences.getString("DNS", ""));
      Serial.println(".local"); 
    }
    digitalWrite(BootLED, HIGH);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/LedOn", []() {
      pinMode(ExtLED, OUTPUT);
      digitalWrite(ExtLED, HIGH);
      Serial.println("LED On");
      server.send(200, "text/html", "LED is ON!");
    });

    server.on("/get-ip", handleGetIP);

    server.on("/LedOff", []() {
      pinMode(ExtLED, OUTPUT);
      digitalWrite(ExtLED, LOW);
      Serial.println("LED Off");
      server.send(200, "text/html", "LED is OFF!");
    });

    server.on("/readADC", handleADC);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
  }
}

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handleADC() {
  int a = digitalRead(ExtLED);
  String valoare = (a == LOW) ? "Oprit" : "Pornit";
  server.send(200, "text/plain", valoare);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String command = (char*)payload;
    if (command == "led-on") {
      pinMode(ExtLED, OUTPUT);
      digitalWrite(ExtLED, HIGH);
      Serial.println("Led turned on (WebSocket)");
      server.send(200, "text/html", "LED is ON!");
    } 
    else if (command == "led-off") {
      pinMode(ExtLED, OUTPUT);
      digitalWrite(ExtLED, LOW);
      Serial.println("Led turned off (WebSocket)");
      server.send(200, "text/html", "LED is OFF!");
    }
    else if (command == "reboot") {
      esp_restart();
    }
    else if (command == "DNS") {
      Serial.print("http://");
      Serial.print(preferences.getString("DNS", ""));
      Serial.println(".local"); 
    }
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    dnsServer.processNextRequest();
    delay(2000);
  }
  server.handleClient();
  webSocket.loop();

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');  // Citește comanda până la newline
    command.trim();  // Elimină eventualele spații albe
    if (command == "admingetip") {
      Serial.print("IP curent: ");
      Serial.println(WiFi.localIP());
    }
    else if (command == "sudo reboot") {
      Serial.println("Restart ESP...");
      delay(1000);
      esp_restart();
    }
    else if (command == "admingetdns") {
      Serial.print("Adresa: http://");
      Serial.print(preferences.getString("DNS", ""));
      Serial.println(":81/");
      delay(1000);
    }
    else if (command == "admingetwifidata") {
      Serial.print("Adresa wifi: ");
      Serial.println(preferences.getString("ssid", user_wifi.ssid));
      Serial.print("Parola: ");
      Serial.println(preferences.getString("password", user_wifi.password));
      delay(1000);
    }
    else if (command == "clearEEPROM") {
      Serial.println("Stergere memorie...");
      preferences.clear();
      delay(2000);
      Serial.println("EEPROM sters! Restart in 5 secunde...");
      delay(5000);
      esp_restart();
    }
    else {
      Serial.print("Comandă necunoscută.");
    }
  }
}

void handleWiFiScan() {
    String json = "[";
    int n = WiFi.scanNetworks();
    
    for (int i = 0; i < n; ++i) {
        if (i > 0) json += ",";
        json += "\"" + WiFi.SSID(i) + "\"";
    }
    json += "]";

    server.send(200, "application/json", json);
}

void handleGetIP() {
    server.send(200, "text/plain", WiFi.localIP().toString());
}

void handlePortal() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));

    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = user_wifi.dns[server.arg("DNS").length()] = '\0';

    preferences.putBytes("ssid", user_wifi.ssid, sizeof(user_wifi.ssid));
    preferences.putBytes("password", user_wifi.password, sizeof(user_wifi.password));
    preferences.putString("DNS", server.arg("DNS").c_str());  // Salvează DNS-ul

    String d = SETUP_success;
    server.send(200, "text/html", SETUP_success);
    delay(5000);
    Serial.println("Settings for WiFi and DNS are saved! RESTARTING!");
    Serial.print("Citit din pagina: ");
    Serial.println(server.arg("DNS").c_str());
    Serial.print("Citit din memorie: ");
    Serial.println(preferences.getString("DNS", ""));
    delay(2000);
    esp_restart();
} else {
    String d = SETUP_page;
    server.send(200, "text/html", d);
}
}