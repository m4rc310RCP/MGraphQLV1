#include "MGraphQL.h"

MGraphQL gql;

void toggleRelay(DynamicJsonDocument& params);

typedef void (*CommandHandler)(DynamicJsonDocument&);
struct Command {
  const char* name;
  CommandHandler handler;
};

Command commands[] = {
  {"ALTERNAR_RELE", toggleRelay}
};

void setup(){
  Serial.begin(115200);

  gql.onSerialPrint([](char* message){Serial.print(message);});



  gql.onEvent([&](_control_event event){
    if (event == MWS_CONNECT_TO_WIFI){
      gql.connectWiFi("Silva", "Dh13ssyc4");
    }else if (event == MWS_REGISTER_SUBSCRIPTION){
      DynamicJsonDocument variables(254);
      variables["serial"] = gql.getDeviceSerialNumber();
      // JsonObject device = variables.createNestedObject("device");
      // device["nr_serie"] = gql.getDeviceSerialNumber();
      // device["nm_fabricante"] = "";
      // device["ds_dispositivo"] = "ESP32S3";

      String query = R"(
        subscription SubControl($serial:String) {
          CONTROLE(nr_serie:$serial){
            ds_acao
            parametro{
              ds_item
              ds_estado
            }
          }
        }
      )";
      //const char* action = doc["action"];

      gql.subscription(variables, query, [&](DynamicJsonDocument data){

        

        const char* action = data["ds_acao"];



        if (!action) {
          gql.mprint("No action supported!");
          return;
        }

        for (Command& command : commands) {
          if (strcmp(command.name, action) == 0) {
            //command.handler(data["parametro"]);
            gql.mprint("Action ~> %s", command.name);
            return;
          }
        }

        gql.mprint("No command supported!");
      });
    }

  });

  gql.setAuthorization("Test test:test");

  gql.begin("dev-thermo.m4rc310.com.br");

}

void toggleRelay(DynamicJsonDocument& params) {
  //int relayId = params["relayId"];
  //const char* state = params["state"];

  gql.mprint("Call action");

  // Implemente a lógica de controle do relé aqui
}



void loop(){
	gql.loop();
}