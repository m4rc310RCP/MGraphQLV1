#include "MGraphQL.h"

MGraphQL::MGraphQL(bool secure){
	this->secure = secure;
	this->client = secure ? new WiFiClientSecure() : new WiFiClient();
	if (secure){
		WiFiClientSecure* secureClient = static_cast<WiFiClientSecure*>(this->client);
		mprint("Disable SSL validation\n");
		secureClient->setInsecure();
	}
}

MGraphQL::~MGraphQL() {
	delete this->client;
}

bool MGraphQL::begin(char* host, int port, char* path){
	// --------------------------------------------------------------------------------
	mprint("\nStartup application ESP32 S/N: %s\n", getDeviceSerialNumber());
	// --------------------------------------------------------------------------------
	onEvent<ConnectionInfo>(MWS_WIFI_CONNECTION_INFO, [&](ConnectionInfo info){
		if(info.sucess){
			mprint("\nWiFi connected in: %s with IP: %s Level: %sdBm (%s)\n", 
				info.ssid, info.ip, String(info.rssi), info.rssiLevel
			);
			//callEvent(MWS_CONNECT_TO_SERVER, client);
			mprint("Connecting to server %s:%s...", host, String(port));
			
			if (client->connect(host, port)){
				mprint("\nConnected!");
				mprint("\nRequest upgrade to WebSocket Protocol...");
			}else{
				mprint("\nConnection error");
			}	


		}else{
			mprint("Erro in connection WiFi: %s", info.messageError);
		}
	});
	// --------------------------------------------------------------------------------
	onEvent<WiFiClient>(MWS_CONNECT_TO_SERVER, [&](WiFiClient _client){
			mprint("Connecting to server %s:%s...", host, String(port));
			
			if (client->connect(host, port)){
				mprint("\nConnected!");
				mprint("\nRequest upgrade to WebSocket Protocol...");
			}else{
				mprint("\nConnection error");
			}		
	});
	// --------------------------------------------------------------------------------

	int32_t timeout = millis() + 5000;
	callEvent(MWS_CONNECT_TO_WIFI, timeout);
	return true;
}

void MGraphQL::mprint(const char *format, ...) {
	  if (_callback_serial_print) {
				va_list args;
				va_start(args, format);
				size_t size = 256; 
				char buffer[size];
				vsnprintf(buffer, size, format, args);
				_callback_serial_print(buffer);
				va_end(args);
    } else {
        Serial.println("** Callback não definido **");
    }
}

String MGraphQL::getDeviceSerialNumber(){
	uint64_t chipId = ESP.getEfuseMac();
	char serialNumber[18];
  snprintf(serialNumber, sizeof(serialNumber), "%04X%08X",(uint16_t)(chipId >> 32), (uint32_t)chipId);
	return String(serialNumber);
}
