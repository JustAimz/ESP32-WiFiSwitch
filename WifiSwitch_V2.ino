#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <LiquidCrystal.h>
#include "index.h"

#define ExtLED 3  // Use a valid GPIO for ESP32-C3
#define BootLED 8

MDNSResponder mdns;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
const char* redirectIP = "172.217.28.1";

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
  digitalWrite(BootLED, HIGH);
  delay(200);
  digitalWrite(BootLED, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  if (preferences.getString("ssid", user_wifi.ssid) != NULL) {
    delay(2000);
    Serial.print("...");
    delay(2000);
    Serial.print("...");
    delay(2000);
    Serial.print("...");
    delay(2000);
    Serial.print("...");
    delay(2000);
    Serial.println("...");
    delay(1000);
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
    delay(200);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);
    delay(100);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);
    delay(100);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);
    delay(100);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);
    delay(100);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);
    delay(100);
    digitalWrite(BootLED, HIGH);
    delay(100);
    digitalWrite(BootLED, LOW);

    Serial.println("Please use WiFi \"SmartLink Outlet\" to setup device");
  } else {
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    delay(5000);
    if (preferences.getString("DNS", user_wifi.dns) != NULL) {
      if (!MDNS.begin(preferences.getString("DNS", user_wifi.dns))) { // Numele pe care îl vei folosi pentru acces
      Serial.println("Eroare la inițializarea mDNS");
      return;
      }
      Serial.println("mDNS inițializat");
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
    else if (command == "clearEEPROM") {
      preferences.clear();
      delay(2000);
      server.send(200, "text/html", "EEPROM CLEARED!");
      Serial.println("EEPROM Cleared! Restarting...");
      delay(5000);
      esp_restart();
    }
    else if (command == "reboot") {
      esp_restart();
    }
    else if (command == "DNS") {
      Serial.println(preferences.getString("DNS", user_wifi.dns));
    }
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    dnsServer.processNextRequest();
  }
  server.handleClient();
  webSocket.loop();
}

void handleGetIP() {
    server.send(200, "text/plain", WiFi.localIP().toString());
}

void handlePortal() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    strncpy(user_wifi.dns, server.arg("DNS").c_str(), sizeof(user_wifi.dns));  // Nou câmp DNS

    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = user_wifi.dns[server.arg("DNS").length()] = '\0';

    preferences.putBytes("ssid", user_wifi.ssid, sizeof(user_wifi.ssid));
    preferences.putBytes("password", user_wifi.password, sizeof(user_wifi.password));
    preferences.putBytes("DNS", user_wifi.dns, sizeof(user_wifi.dns));  // Salvează DNS-ul

    server.send(200, "text/html", "<html><head><style>"
                                 "body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; padding: 20px; }"
                                 "h1 { color: #333; }"
                                 "form { background-color: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }"
                                 "input[type='text'], input[type='password'], input[type='text'] { width: 250px; padding: 8px; margin: 10px 0; border-radius: 4px; border: 1px solid #ccc; }"
                                 "button { padding: 10px 20px; border-radius: 5px; background-color: #4CAF50; color: white; border: none; cursor: pointer; }"
                                 "button:hover { background-color: #45a049; }"
                                 "</style></head><body><h1>Wifi Setup</h1><p>Settings saved! Restarting device in 5 seconds.</p></body></html>");
    delay(5000);
    Serial.print("Settings for WiFi and DNS are saved! RESTARTING!");
    delay(2000);
    esp_restart();
} else {
    server.send(200, "text/html", "<html><head><style>"
                                 "body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; padding: 20px; }"
                                 "h1 { color: #333; }"
                                 "form { background-color: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }"
                                 "input[type='text'], input[type='password'], input[type='text'] { width: 250px; padding: 8px; margin: 10px 0; border-radius: 4px; border: 1px solid #ccc; }"
                                 "button { padding: 10px 20px; border-radius: 5px; background-color: #4CAF50; color: white; border: none; cursor: pointer; }"
                                 "button:hover { background-color: #45a049; }"
                                 "</style></head><body><h1>Wifi Setup</h1>"
                                 "<form action='/' method='post'>"
                                 "WiFi Name: <input type='text' name='ssid'><br>"
                                 "Password: <input type='password' name='password'><br>"
                                 "DNS: <input type='text' name='DNS'><br>"
                                 "<button type='submit'>Save</button>"
                                 "</form></body></html>");
}
}