#include "MGraphQL.h"
#include <WiFiClientSecure.h>
#include <WiFi.h>
#define CRLF "\r\n"

MGraphQL::MGraphQL(bool secure){
	this->secure = secure;
	this->client = secure ? new WiFiClientSecure : new WiFiClient;
	// if (secure){
	// 	this->client = new WiFiClientSecure();
	// 	((WiFiClientSecure)client).setInsecure();
	// }else{
	// 	this->client = new WiFiClient();
	// }


	this->_callbackControlEvent = [](auto) {};
}

MGraphQL::~MGraphQL() {
	delete this->client;
}

void MGraphQL::callEvent(_control_event event){
	if(_callbackControlEvent != nullptr){
		_callbackControlEvent(event);
	}
}

bool MGraphQL::begin(char* host, int port, char* path){
	_host = host;
	_port = port;
	_path = path;

	connectingWiFi = true;
	return true;
}

void MGraphQL::requestWsUpdate(){
	mprint("\nSending websocket upgrade headers\n");

	if (this->secure) {
    WiFiClientSecure* secureClient = static_cast<WiFiClientSecure*>(this->client);
    secureClient->setInsecure();
  }

	if (client->connect(_host, _port)){
		char buf[1024];
		sprintf_P(buf, PSTR("GET %s HTTP/1.1\r\n"
												"Host: %s\r\n"
												"Upgrade: websocket\r\n"
												"Connection: Upgrade\r\n"
												"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
												"Origin: https://%s\r\n"
												"Sec-WebSocket-Protocol: graphql-ws\r\n"
												"Sec-WebSocket-Version: 13\r\n\r\n"),
		_path, _host, _host);

		int valread = 0;
		String temp;
    int bite;
		bool wsupdated = false;
		bool endOfResponse = false;
		String s;
		client->write((uint8_t*)buf, strlen(buf)); 
		while (!endOfResponse && (s = client->readStringUntil('\n')).length() > 0) {
			if (s.indexOf("HTTP/") != -1) {
				auto status = s.substring(9, 12);
				if (status == "101"){
					wsupdated = true;
				}else {
					wsupdated = false;
				}
			} else if (s == "\r"){
				endOfResponse = true;
			}
		}

		if (wsupdated){
			mprint("\nWebsocket upgrade headers success!\n");
			handshake();
		}
	}else{
		mprint("\nUnable to connect to the server\n");
	}
}


void MGraphQL::onEvent(std::function<void(_control_event)> event){
	this->_callbackControlEvent = event;
}

void MGraphQL::onSerialPrint(std::function<void(char*)> callback){
	this->_callbackSerialPrint = callback;
}
void MGraphQL::mprint(char* format, ...){
	if(this->_callbackSerialPrint != nullptr){
		va_list args;
		va_start(args, format);
		size_t size = 256; 
		char buffer[size];
		vsnprintf(buffer, size, format, args);
		_callbackSerialPrint(buffer);
		va_end(args);
	}
}

void MGraphQL::connectWiFi(char * wifi_ssid, char * wifi_password){
	mprint("Connecting for WiFi: %s ", wifi_ssid);
	WiFi.begin(wifi_ssid, wifi_password);
	unsigned long limitMillis = millis() + TIMEOUT;
	bool connected = false;
	while (millis() <= limitMillis){
		if (WiFi.status() == WL_CONNECTED){
				connected = true;
				connectingWiFi = true;
				break;
		}else{
			mprint(".");
		}
		delay(1000);
	}
	if (connected){
		mprint("\nWiFi connect in %s with IP: %s \n", wifi_ssid, WiFi.localIP().toString());
		requestWsUpdate();
	}else{
		mprint("\nError in connetion WiFi for timeout\n");
	}
	
}

void MGraphQL::loop(void){
	if(connectingWiFi && WiFi.status() != WL_CONNECTED){
		connectingWiFi = false;
		callEvent(MWS_CONNECT_TO_WIFI);
	}else{
		if (canRead && client->available()){
			//canRead = false;
			String msg;
			if (getMessage(msg)){
				mprint("Handshake: %s \n", msg.c_str());
			}
		}
	}
}

void MGraphQL::mwrite(const char* buffer, std::function<void(String response)> payload) {
    if (client->connected()) { 
			client->write((uint8_t*)buffer, strlen(buffer)); 
			unsigned long timeout = millis() + 10000;
			int valread = 0;
			while (millis() < timeout) {
				if (client->available()) {
					String temp;
    			int bite;
					while ((bite = client->read()) != -1) {
						temp += (char)bite;
						if ((char)bite == '\n') {
							payload(temp);
						}
					}


					// char buf[1024];
					// valread = client->readBytes((uint8_t*)buf, sizeof(buf));
					// buf[valread] = '\0';
					// payload(buf); 
					break;
				}
			}
			if (valread == 0) {
				mprint(PSTR("Timeout esperando pela resposta do servidor"));
				return;
			}
    }
}

bool MGraphQL::handshake(){
	String payload = R"({"type":"connection_init", "payload":{"Authorization": "Test test:test"}})";
	// String payload = R"(char *payloadStart = R"({"id":"1","type":"start","payload":{"variables":{},"extensions":{},"operationName":"TEST","query":"subscription {\n  mostrarInformacoesServidor {\n    dt_pulso\n  }\n}\n"}})";
	send(payload);
	int timeout = millis() + 10000;
	bool response = false;
	while(millis() <= timeout){
		String msg;
		if (getMessage(msg)){
			DynamicJsonDocument doc(256);
			DeserializationError error = deserializeJson(doc, msg);
			response = true;
			if (error){
				mprint("Error in convert json response ACK: %s", error.c_str());
				response = false;
			}else{
				const char* type = doc["type"];
				if (strcmp(type, "connection_ack") == 0) {
					mprint("Resp: %s", type);
					String paydata = R"({"id":"1","type":"start","payload":{"variables":{},"extensions":{},"operationName":"ServerInfo","query":"subscription ServerInfo {\n  mostrarInformacoesServidor {\n    dt_pulso\n  }\n}\n"}})";
					send(paydata);
					canRead = true;
				}
			}
			break;
		}
		delay(100);
	}
	
	//canRead = true;
	
	// const char *buff = spayload.c_str();
	// mprint("Waiting for handshake.\n");
	// mwrite(buff, [&](String msg){
	// 	mprint("Handshake: %s \n", msg.c_str());
	// });
	return true;
}

std::string formatString(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    size_t size = std::vsnprintf(nullptr, 0, format.c_str(), args) + 1; // +1 para o caractere nulo
    va_end(args);
    std::vector<char> buffer(size);
    va_start(args, format);
    std::vsnprintf(buffer.data(), size, format.c_str(), args);
    va_end(args);
    return std::string(buffer.data(), buffer.size() - 1); // -1 para remover o caractere nulo
}

void MGraphQL::send(const String& str){
	client->write(WS_FIN | WS_OPCODE_TEXT);
	int size = str.length();
	if (size > 125) {
		client->write(WS_MASK | WS_SIZE16);
		client->write((uint8_t) (size >> 8));
		client->write((uint8_t) (size & 0xFF));
	} else {
		client->write(WS_MASK | (uint8_t) size);
	}

	// 3. send mask
	uint8_t mask[4];
	mask[0] = random(0, 256);
	mask[1] = random(0, 256);
	mask[2] = random(0, 256);
	mask[3] = random(0, 256);

	client->write(mask[0]);
	client->write(mask[1]);
	client->write(mask[2]);
	client->write(mask[3]);

	//4. send masked data
	for (int i = 0; i < size; ++i) {
		client->write(str[i] ^ mask[i % 4]);
	}
}

bool MGraphQL::getMessage(String& message){
	if (!client->connected()) {	return false; }
	if(!client->available()){
		return false;
	}
	// 1. read type and fin
	unsigned int msgtype = timedRead();
	if (!client->connected()) {return false;}

	// 2. read length and check if masked
	int length = timedRead();
	bool hasMask = false;
	if (length & WS_MASK) {
		hasMask = true;
		length = length & ~WS_MASK;
	}

	if (length == WS_SIZE16) {
		length = timedRead() << 8;
		length |= timedRead();
	}

	// 3. read mask
	if (hasMask) {
		uint8_t mask[4];
		mask[0] = timedRead();
		mask[1] = timedRead();
		mask[2] = timedRead();
		mask[3] = timedRead();

		// 4. read message (masked)
		message = "";
		for (int i = 0; i < length; ++i) {
			message += (char) (timedRead() ^ mask[i % 4]);
		}
	} else {
		// 4. read message (unmasked)
		message = "";
		for (int i = 0; i < length; ++i) {
			message += (char) timedRead();
		}
	}
	return true;
}

int MGraphQL::timedRead(){
	while (!client->available()) {
		delay(20);
	}
	return client->read();
}

		// void MGraphQL::write(uint8_t data){
		// 	if (client->connected()){
		// 		client->write(data, strlen(data)); 
		// 	}
		// }
		
		// void MGraphQL::write(char * data){
		// 	write((uint8_t)data); 
		// }