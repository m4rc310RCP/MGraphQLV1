#include "MGraphQL.h"
#include "env.h"

char* host = "dev-thermo.m4rc310.com.br";

IPAddress primaryDNS(8, 8, 8, 8);    // Google DNS
IPAddress secondaryDNS(8, 8, 4, 4);  // Google DNS Secundário (opcional)


MGraphQL gql;

void setup(){
  Serial.begin(115200);
  delay(10);
  if (Serial){
    gql.setSerialListener([&](char *message){
      Serial.print(message);
    });
  }

  gql.onEvent<int32_t>(MWS_CONNECT_TO_WIFI, [&](int32_t timeout){
    
    ConnectionInfo info;
    info.sucess = false;

    WiFi.begin(wifi_ssid, wifi_password);
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), primaryDNS, secondaryDNS);
    while(millis() <= timeout && !info.sucess){
      gql.mprint(".");
      info.sucess = WiFi.status() == WL_CONNECTED;
      delay(1000);
    }

    if (info.sucess){
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
    }else{
      info.messageError = "Timeout";
    }
    gql.callEvent(MWS_WIFI_CONNECTION_INFO, info);
  });

  
  gql.begin("dev-thermo.m4rc310.com.br");
}

void loop(){}