#include "MGraphQL.h"

const char* ssid          = "Silva";
const char* ssid_password = "Dh13ssyc4";
char* host = "dev-thermo.m4rc310.com.br";
char* path = "/graphql";
int port    = 443;

MGraphQL gql;

void setup(){
  Serial.begin(115200);
  delay(10);
  if (Serial){
    gql.setSerialListener([&](char *message){
      Serial.print(message);
    });
  }
  
  gql.begin("dev-thermo.m4rc310.com.br", 443, "/graphql");
}

void loop(){}