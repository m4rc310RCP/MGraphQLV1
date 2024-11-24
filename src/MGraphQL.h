#ifndef MGRAPHQL_H
#define MGRAPHQL_H

//#include <iostream>
#include <functional> 
#include <map> 
#include "../libs/ArduinoJson.h"
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
//#include <JsonObject.h>
#include <WiFi.h>
//#include <esp_wifi.h>

#include <regex>



#define TIMEOUT 5000
#define MAX_CONNECTION_ATTEMPTS 5

#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02

#define WS_MASK           0x80
#define WS_SIZE16         126

typedef enum {
	MWS_CONNECT_TO_WIFI      = 0,
	MWS_CONNECT_TO_SERVER    = 1,
	MWS_CONNECT_TO_WS        = 2,
	MWS_PROCESS_ACK          = 3,
	MWS_INIT_SUBSCRIPTIONS   = 4,
	MWS_INIT_STREAMING       = 5,
	MWS_WIFI_CONNECTION_INFO = 6
} mws_event_t;

typedef enum {
	//MWS_CONNECT_TO_WIFI = 1,
	//MWS_CONNECTING_TO_WIFI_TIMEOUT = 2
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
		int subscriptionId = 0;

		using ActionFunction = std::function<void(JsonObject)>;
		std::map<String, ActionFunction> _action_functions;

		bool canRead = false;
		bool inHeartBeat = false;

		std::function<void(char*)> _callbackSerialPrint = nullptr;
		//std::function<void(_control_event)> _callbackControlEvent = nullptr;

		void mwrite(const char* buffer, std::function<void(String response)> payload);
		bool handshake();

		void send(const String& message);
		bool getMessage(String& message);
		int timedRead();


		std::map<mws_event_t, std::function<void(void*)>> _cb_event; 
		std::map<String, std::function<void(void*)>> _cb_string_event; 

		void subscription(String& id, String& query, String& variables,  std::function<void(String)> data);
		String formatJson(String jsonString);
		// void write(uint8_t data);
		// void write(char* data);

	public:
		MGraphQL(bool secure = true);
		~MGraphQL();
		bool begin(char* host, int port = 443, char* path = "/graphql");
		//void onEvent(std::function<void(_control_event)> event);
		//void callEvent(_control_event event);
		void onSerialPrint(std::function<void(char*)> callback);
		int32_t heartbeatMillis = -1;
		void mprint(char* format, ...);
		std::string formatString(const std::string& format, ...);
		void setAuthorization(char* authorization) {this->_authorization = authorization;};

		void subscription(String& query, String& variables,  std::function<void(String)> data);

		void connectWiFi(char * wifi_ssid, char * wifi_password);
		void reconnect();

		void requestWsUpdate();
		void loop(void);

		String extractOperationName(const String& query); 

		void registerAction(const String& actionKey, ActionFunction action) {
			this->_action_functions[actionKey] = action;
		};

		void callAction(const String& actionKey, JsonObject data);

	// Função para registrar o callback
    template <typename T>
    void onEvent(mws_event_t event, std::function<void(T)> callback) {
        this->_cb_event[event] = [callback](void* param) {
            callback(*static_cast<T*>(param));
        };
    }

    // Especialização para o tipo void
    template <typename T>
    void onEvent(String id, std::function<void(T)> callback) {
			this->_cb_string_event[id] = [callback](void* param){
				callback(*static_cast<T*>(param));
			};
    }

    void onEvent(mws_event_t event, std::function<void()> callback) {
        this->_cb_event[event] = [callback](void*) {
            callback();
        };
    }

    // Função para chamar o evento
    template <typename T>
    void callEvent(mws_event_t event, T value) {
        if (_cb_event.find(event) != _cb_event.end()) {
            _cb_event[event](&value);
        }
    }

    // Sobrecarga para o tipo void
		template <typename T>
    void callEvent(String id, T value) {
        if (_cb_string_event.find(id) != _cb_string_event.end()) {
            _cb_string_event[id](&value);
        }
    }

    void callEvent(mws_event_t event) {
        if (_cb_event.find(event) != _cb_event.end()) {
            _cb_event[event](nullptr);
        }
    }

		void startSubscriptions();
};
#endif