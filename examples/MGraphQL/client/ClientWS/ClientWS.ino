#include <MGraphQL.h>


MGraphQL gql;

void setup(){
  Serial.begin(115200);

  gql.listenerPrintEvent([&](char *text){
    Serial.print(text);
  });

  gql.begin("dev-thermo.m4rc310.com.br");

}

void loop(){
	
}
