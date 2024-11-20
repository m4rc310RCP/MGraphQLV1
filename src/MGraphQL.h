
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
	MWS_CONNECT_TO_WIFI      = 0,
	MWS_CONNECT_TO_SERVER    = 1,
	MWS_CONNECT_TO_WS        = 2,
	MWS_PROCESS_ACK          = 3,
	MWS_INIT_SUBSCRIPTIONS   = 4,
	MWS_INIT_STREAMING       = 5,
	MWS_WIFI_CONNECTION_INFO = 6
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
		char* _host;
		char* _path;
		int   _port;

	

		std::function<void(char *message)> _callback_serial_print = nullptr;
		std::map<mws_event_t, std::function<void(void*)>> _cb_event; 
	public:
		MGraphQL(bool secure = true);
		~MGraphQL();

		bool secure;
		WiFiClient *client;
		//~> Begin GraphQL ------------------------------------------------//
		bool begin(char* host, int port = 433, char* path = "/graphql");
		void setSerialListener(std::function<void(char *message)> callback) { this->_callback_serial_print = callback; };
    void mprint(const char *format, ...);
		String getDeviceSerialNumber();
		//~> Begin GraphQL ------------------------------------------------//
		template <typename T>
		void onEvent(mws_event_t event, std::function<void(T)> callback) {
				this->_cb_event[event] = [callback](void* param) {
					callback(*static_cast<T*>(param));
				};
		};

		template <typename T>
		void callEvent(mws_event_t event, T value){
			if (_cb_event.find(event) != _cb_event.end()){
				_cb_event[event](&value);
			}
			// auto it = _cb_event.find(event);
			// if (it != _cb_event.end()){
			// 	it->second(value);
			// }else{
			// 	mprint("No callback for event.");
			// }
		};

		// template <typename T>
		// void callEvent(mws_event_t event, T value);
		//~> Begin GraphQL ------------------------------------------------//
		
};

#endif