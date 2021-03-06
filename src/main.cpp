#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ProcessScheduler.h>

#include "./tasks/OTAUpdateProcess.cpp"
#include "./tasks/WebServerProcess.cpp"
#include "./tasks/WebSocketServerProcess.cpp"
#include "config.h"
Scheduler sched;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "start123";

void setupWiFiAP(){
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 WIFI CAR " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void setupWifiClient(){
  const char* ssid = "Andrewhous";//"Andrewhous";
  const char* password = "1234567891111";//"1234567891111";

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  const uint8_t MAX_TRYING_COUNT = 1;
  uint8_t trying = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    trying++;
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    //if ( trying > MAX_TRYING_COUNT ) {
      setupWiFiAP();
      break;
    // } else {
    //   ESP.restart();
    // }

  }
}

OTAUpdateProcess otaUpdate(sched,HIGH_PRIORITY,250,"0819");
WebServerProcess webServer(sched,HIGH_PRIORITY,250);
WebSocketServerProcess webSocketServer(sched,HIGH_PRIORITY,250);

void setup() {
  Serial.begin(115200);
  setupWiFiAP();
  //setupWifiClient();
  Serial.println(WiFi.localIP());
  otaUpdate.add(true);
  webServer.add(true);
  webSocketServer.add(true);
}
void loop() {
  sched.run();
}
