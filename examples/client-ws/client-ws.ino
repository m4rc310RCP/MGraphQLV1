#include "MGraphQL.h"

 MGraphQL gql;

void setup(){
  Serial.begin(115200);
  
  gql.addPrintListener([](const std::string& message) {
    Serial.print(message.c_str());
  });
  // gql.addPrintListener([](const std::string& message){
  //   Serial.print(message.c_str());
  // });

  //mprint("init...");

  gql.begin("dev-thermo.m4rc310.com.br");
}

void loop(){
	// unsigned long currentMillis = millis();
	// if (millis() % 1500 == 0){
  //   Serial.print(".");
	// }
}