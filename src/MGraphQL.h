
#ifndef MGRAPHQL_H
#define MGRAPHQL_H

#include <Arduino.h>
#include <functional>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <cstdio>
#include <functional>
#include <map>
#define PSTR(s)  (s) 
#include <pgmspace.h> 
#include <cstdarg>


typedef enum {
	MWS_CONNECT_WIFI = 0,
	MWS_CONNECT_WS = 1,
	MWS_STREAMING_MESSAGE = 2,
	MWS_CONNECT_WIFI_OK = 3
} mws_event_t;

struct  ConnectionInfo{
	bool sucess;
	String ssid; 
	String ip;
	int32_t rssi;
	String rssiLevel;
	String messageError;
};

class MGraphQL{
	private:
		WiFiClientSecure client;
		std::map<mws_event_t, std::function<void(void*)>> _CB_ENVET_GLOBAL; 
	public:
		bool begin(char* host, int port = 433, char* path = "/graphql");
		WiFiClientSecure getClient(){return client;};
		//~> General CALLBACK ------------------------------------------//
		template <typename T>
		void onEvent(mws_event_t event, std::function<void(T)> callback){
			this->_CB_ENVET_GLOBAL[event] = [callback] (void * param){
				callback(*static_cast<T*>(param));
			};
		};

		template <typename T>
		void callEvent(mws_event_t event, T value) {
				auto it = this->_CB_ENVET_GLOBAL.find(event);
				if (it != this->_CB_ENVET_GLOBAL.end() && it->second != nullptr) {
						T* ptrValue = &value;
						if (ptrValue) {
								it->second(static_cast<void*>(ptrValue));
						}
				}
		}
		//~> General CALLBACK ---------------------------------------------//
		void mprint(const char *format, ...);
		//-----------------------------------------------------------------//
};

#endif