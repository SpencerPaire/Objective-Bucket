#ifndef WebServer_h
#define WebServer_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include "GameModeRunner.h"

#define AP_SSID     "Objective Bucket"
#define AP_HOSTNAME "ob"
#define AP_IP_ADDR  IPAddress(192, 168, 4, 1)

class OBWebServer {
private:
  AsyncWebServer server;
  GameModeRunner *runner;
  void setupRoutes();
  String buildStateJson();

public:
  OBWebServer(GameModeRunner *runner);
  void Begin();
  void Update();
};

#endif
