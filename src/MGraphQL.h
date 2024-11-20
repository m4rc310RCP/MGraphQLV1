
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
	MWS_CONNECT_TO_WIFI    = 0,
	MWS_CONNECT_TO_SERVER  = 1,
	MWS_CONNECT_TO_WS      = 2,
	MWS_PROCESS_ACK        = 3,
	MWS_INIT_SUBSCRIPTIONS = 4,
	MWS_INIT_STREAMING     = 5
} mws_event_t;

class MGraphQL{
	private:
		char* _host;
		char* _path;
		int   _port;
		std::function<void(char *message)> _callback_serial_print = nullptr;
	public:
		//~> Begin GraphQL ------------------------------------------------//
		bool begin(char* host, int port = 433, char* path = "/graphql");
		void setSerialListener(std::function<void(char *message)> callback) {this->_callback_serial_print = callback;};
    void mprint(const char *format, ...);
		String getDeviceSerialNumber();
		//~> Begin GraphQL ------------------------------------------------//
		
};

#endif