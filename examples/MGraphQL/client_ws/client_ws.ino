#include "MGraphQL.h"
#include "env.h"


MGraphQL gql;

ConnectionInfo info;

void setup(){
  Serial.begin(115200);
  if (Serial){
    
    gql.onEvent<char*>(MWS_STREAMING_MESSAGE, [&](char * message){
      Serial.println(message);
    });

    gql.mprint("Connecting to WiFi %s...", ssid);
    
    info.millisWiFiConnection = millis() + TIMEOUT;
    bool isConnected = false;
    WiFi.begin(ssid, pass);
    while(millis() <= info.millisWiFiConnection && !isConnected){
      isConnected = WiFi.status() == WL_CONNECTED;
      gql.mprint(".");
      delay(1000);
    }

    if (isConnected){
      info.ip   = WiFi.localIP().toString();
      info.ssid = WiFi.SSID();
      info.rssi = WiFi.RSSI();
      if (info.rssi > -50) {
        info.rssiLevel = "Excellent";
      } else if (info.rssi > -70) {
          info.rssiLevel = "Good";
      } else if (info.rssi > -85) {
          info.rssiLevel = "Weak";
      } else {
          info.rssiLevel = "Poor";
      }

      info.serialNumber = gql.getDeviceSerialNumber();

      gql.mprint("Connected in WiFi SSID: %s IP: %s %sdBm - %s", info.ssid, info.ip, String(info.rssi), info.rssiLevel);
      info.millisWiFiConnection = millis()-1;
      gql.upgradeToWS(host, port, path);
    }else{
      if (++info.countTryConnection <= 5){
        info.millisWiFiConnection = millis() + TIMEOUT;
        gql.mprint("trying %s of %s...", String(info.countTryConnection), "5");
      }else{
        info.countTryConnection = 0;
        info.millisWiFiConnection = millis() + (10*60*1000);
        gql.mprint("Fail in connection WiFi. Try again in 10 minutes...");
      }
    }
  }
}

void loop(){
  gql.loop();
}