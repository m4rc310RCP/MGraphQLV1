
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

#define TIMEOUT 10000; 

#define MAX_CONNECTION_ATTEMPTS 5

#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02

#define WS_MASK           0x80
#define WS_SIZE16         126


typedef enum {
	MWS_CONNECT_WIFI = 0,
	MWS_CONNECT_WS = 1,
	MWS_STREAMING_MESSAGE = 2,
	MWS_CONNECT_WIFI_OK = 3
} mws_event_t;

struct ConnectionInfo{
	bool isStarted;
	int  countTryConnection;
	int32_t millisWiFiConnection;
	int32_t millisServerConnection;
	String serialNumber;
	String ssid; 
	String ip;
	int32_t rssi;
	String rssiLevel;
	String messageError;
};

class MGraphQL{
	private:
		WiFiClientSecure client;
		ConnectionInfo info;
		const char* _ssid; 
		const char* _pass; 
		const char* _host; 
		int _port; 
		const char* _path;
		bool canRead = false;
		std::map<mws_event_t, std::function<void(void*)>> _CB_ENVET_GLOBAL; 
	public:
		bool connectWiFi();
		bool begin(const char* ssid, const char* pass, const char* host, int port = 433, const char* path = "/graphql");
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
		String getDeviceSerialNumber();
		void upgradeToWS(const char * host, int port, const char * path);
		void loop(void);
		bool handshake();
		void send(const String& message);
		bool getMessage(String& message);
		int timedRead();
};

#endif