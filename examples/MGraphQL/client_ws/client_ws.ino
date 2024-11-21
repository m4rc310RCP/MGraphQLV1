#include "MGraphQL.h"

const char* ssid          = "Silva";
const char* ssid_password = "Dh13ssyc4";
char* host = "dev-thermo.m4rc310.com.br";
char* path = "/graphql";
int port    = 443;

MGraphQL gql;

void setup(){
  Serial.begin(115200);


  gql.onEvent<char*>(MWS_STREAMING_MESSAGE, [&](char * message){
    Serial.println(message);
  });

    ConnectionInfo info;
    info.sucess = false;

    WiFi.begin(ssid, ssid_password);
    //WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), primaryDNS, secondaryDNS);
    while(WiFi.status() != WL_CONNECTED){
      gql.mprint(".");
      delay(1000);
    }

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
    
    gql.mprint("WiFi connected IP: ", info.ip);

  // gql.onEvent<int32_t>(MWS_CONNECT_WIFI, [&](int32_t timeout){

  //   gql.callEvent(MWS_CONNECT_WIFI_OK, info);
  // });


  // gql.setSerialListener([&](char * message){
  //   Serial.print(message);
  // });

  // gql.setGraphQLEvent([&](mws_event_t event){
  //   if (event == MWS_CONNECT_WIFI){
  //     Serial.print("Connecting to WiFi");
  //   }
  // });

  // gql.setSerialListener([&](char * message){
  //   Serial.print(message);
  // });
  delay(10);

  gql.begin("dev-thermo.m4rc310.com.br", 443, "/graphql");

//  WiFi.begin(ssid, password);
  
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Serial.println("");
  // Serial.println("WiFi connected");  
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  // delay(1500);

  // client.setInsecure(); 
  // if (client.connect(host, port)) {
  //   Serial.println("Connected");
  // } else {
  //   Serial.println("Connection failed.");
  //   while(1) {}
  // }

  // if (gql.begin(client, host, port, path)) {
  //   Serial.println("Handshake successful");
  // } else {
  //   Serial.println("Handshake failed.");
  //   while(1) {
  //     // Hang on failure
  //   }  
  // }
}

void loop(){}