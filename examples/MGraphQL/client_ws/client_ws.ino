#include "MGraphQL.h"

const char* ssid          = "Silva";
const char* ssid_password = "Dh13ssyc4";
char* host = "dev-thermo.m4rc310.com.br";
char* path = "/graphql";
int port    = 443;

MGraphQL gql;

void setup(){
  Serial.begin(115200);

	 WiFi.begin(ssid, ssid_password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  gql.setSerialListener([&](char * message){
    Serial.print(message);
  });

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