#include "MGraphQL.h"

#define PARAM_NONE   "NONE"
#define INTERNAL_LED "LED_INTERNO"
#define SWITCH_LED   "ALTERNAR_RELE"

#define STATE_ON  "LIGADO"
#define STATE_OFF "DESLIGADO"


MGraphQL gql;

void control_led(JsonObject param){
  String item  = param["ds_item"]  | PARAM_NONE;
  String state = param["ds_estado"] | PARAM_NONE;
  Serial.println("Led called " + item + "  " + state);
  if (item == INTERNAL_LED){
    digitalWrite(LED_BUILTIN, state == STATE_ON ? HIGH : LOW);
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

	gql.registerAction(SWITCH_LED, control_led);

  gql.onSerialPrint([](char* message){Serial.print(message);});

  gql.onEvent(MWS_INIT_SUBSCRIPTIONS, [=](){
    gql.mprint("Registry subscriptions...\n");

    String query = R"(
      subscription WS_CONTROL($serial: String) {
        CONTROLE(nr_serie: $serial) {
          ds_acao
          parametro {
            ds_item
            ds_estado
          }
        }
      }
    )";

    String variables = R"({"serial": "3005EBAE3D98"})";
    

    gql.subscription(query, variables, [&](String json){
      gql.mprint("\nSubs: %s", json.c_str());
      DynamicJsonDocument doc(512);
      // DeserializationError error = deserializeJson(doc, json);
      // if (error) {
      //     Serial.print("Erro ao analisar JSON: ");
      //     Serial.println(error.c_str());
      //     return;
      // }
      // JsonObject dataAction = doc["CONTROLE"];
      // String action     = dataAction["ds_acao"];
      // JsonObject params = dataAction["parametro"];
      // gql.callAction(action, params);
    });


//formatString

    // String query = R"(
    //   subscription{
    //     mostrarInformacoesServidor{
    //       dt_pulso
    //       nr_versao
    //     }
    // })";
    // String variables = "";
    // gql.subscription(query, variables, [&](String jsonData){
    //   //gql.mprint("....");
    //   gql.mprint("Subscriptions response: %s", jsonData);
    // });
  });

	//-- Examples --//
  // gql.onEvent<String>(MWS_INIT_STREAMING, [&](String json){

  //   Serial.println("Steaming...");

  //   DynamicJsonDocument doc(512);
  //   DeserializationError error = deserializeJson(doc, json);
  //   if (error) {
  //       Serial.print("Erro ao analisar JSON: ");
  //       Serial.println(error.c_str());
  //       return;
  //   }

  //   JsonObject dataAction = doc["CONTROLE"];
  //   String action     = dataAction["ds_acao"];
  //   JsonObject params = dataAction["parametro"];

  //   gql.callAction(action, params);
  // });

	//-- Examples --//

  gql.onEvent(MWS_CONNECT_TO_WIFI, [&](){
    gql.connectWiFi("Silva", "Dh13ssyc4");
  });

  gql.setAuthorization("Test test:test");

  gql.begin("dev-thermo.m4rc310.com.br");
}




void loop(){
	gql.loop();
}