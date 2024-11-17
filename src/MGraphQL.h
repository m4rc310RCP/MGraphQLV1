#ifndef MGRAPHQL_H
#define MGRAPHQL_H

#include <Arduino.h>
#include <iostream>
#include <functional> 
#include <WiFiClient.h>
#include <ArduinoJson.h>


#define TIMEOUT 5000
#define MAX_CONNECTION_ATTEMPTS 5

#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02

#define WS_MASK           0x80
#define WS_SIZE16         126

typedef enum {
	MWS_CONNECT_TO_WIFI = 1,
	MWS_CONNECTING_TO_WIFI_TIMEOUT = 2
} _control_event;

class MGraphQL {
	private:
		bool secure;
		bool connectingWiFi = false;
		WiFiClient *client;

		char* _host; 
		char* _path;
		int   _port; 
		char* _authorization;
		int connectionAttempts = 0;

		bool canRead = false;

		std::function<void(char*)> _callbackSerialPrint = nullptr;
		std::function<void(_control_event)> _callbackControlEvent = nullptr;

		void mwrite(const char* buffer, std::function<void(String response)> payload);
		bool handshake();
		void send(const String& message);
		bool getMessage(String& message);
		int timedRead();
		// void write(uint8_t data);
		// void write(char* data);

	public:
		MGraphQL(bool secure = true);
		~MGraphQL();
		bool begin(char* host, int port = 443, char* path = "/graphql");
		void onEvent(std::function<void(_control_event)> event);
		void callEvent(_control_event event);
		void onSerialPrint(std::function<void(char*)> callback);
		void mprint(char* format, ...);
		std::string formatString(const std::string& format, ...);
		void setAuthorization(char* authorization) {this->_authorization = authorization;};

		void connectWiFi(char * wifi_ssid, char * wifi_password);
		void requestWsUpdate();

		void loop(void);
};
#endif