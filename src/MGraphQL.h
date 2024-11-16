
#ifndef MGRAPHQL_H
#define MGRAPHQL_H

#include <Arduino.h>
#include <functional>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <cstdio>
#include <functional>
#define PSTR(s)  (s) 
#include <pgmspace.h> 
#include <cstdarg>


typedef enum {
	MWS_CONNECT_WIFI = 0,
	MWS_CONNECT_WS = 1
} mws_event_t;

class MGraphQL{
	private:
		char* _host;
		char* _path;
		int _port;
		mws_event_t _mws_event;
		std::function<void(mws_event_t event)> _callback_ws_event = nullptr;
		std::function<void(char *message)> _callback_serial_print = nullptr;
	public:
		//~> Begin GraphQL ------------------------------------------------//
		bool begin(char* host, int port = 433, char* path = "/graphql");
		//~> WS event -----------------------------------------------------//
		void setGraphQLEvent(std::function<void(mws_event_t event)> callback){ this->_callback_ws_event = callback; };
		//~> Messages for Serial ------------------------------------------//
		void setSerialListener(std::function<void(char *message)> callback);
    void mprint(const char *format, ...);
		//-----------------------------------------------------------------//
};

#endif