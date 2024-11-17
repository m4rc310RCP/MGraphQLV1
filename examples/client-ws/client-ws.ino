#include "MGraphQL.h"

MGraphQL gql;

void setup(){
  Serial.begin(115200);

  gql.onSerialPrint([](char* message){Serial.print(message);});

  gql.onEvent([&](_control_event event){
    if (event == MWS_CONNECT_TO_WIFI){
      gql.connectWiFi("Silva", "Dh13ssyc4");
    }
  });

  gql.setAuthorization("Test test:test");

  gql.begin("dev-thermo.m4rc310.com.br");
}

void loop(){
	gql.loop();
}