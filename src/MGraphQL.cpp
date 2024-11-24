#include "MGraphQL.h"

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


	//this->_callbackControlEvent = [](auto) {};
}

MGraphQL::~MGraphQL() {
	delete this->client;
}

// void MGraphQL::callEvent(_control_event event){
// 	if(_callbackControlEvent != nullptr){
// 		_callbackControlEvent(event);
// 	}
// }

bool MGraphQL::begin(char* host, int port, char* path){
	_host = host;
	_port = port;
	_path = path;

	connectingWiFi = true;
	return true;
}

void MGraphQL::requestWsUpdate(){
	mprint(PSTR("\nSending websocket upgrade headers\n"));

	if (this->secure) {
    WiFiClientSecure* secureClient = static_cast<WiFiClientSecure*>(this->client);
    secureClient->setInsecure();
  }

	if (client->connect(_host, _port)){
		char buf[256];
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
			mprint(PSTR("\nWebsocket upgrade headers success!\n"));
			if(!handshake()){
				mprint(PSTR("\nWebsocket no handshake. Trying reconnect websocket...\n"));
				reconnect();
			}
		}
	}else{
		mprint(PSTR("\nUnable to connect to the server\n"));
	}
}

// void MGraphQL::onEvent(std::function<void(_control_event)> event){
// 	this->_callbackControlEvent = event;
// }

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

	int32_t currentMillis = millis();

	if (currentMillis > this->heartbeatMillis && this->client->connected()){
		inHeartBeat = false;
		reconnect();
		return;
	}

	if(connectingWiFi && WiFi.status() != WL_CONNECTED){
		connectingWiFi = false;
		callEvent(MWS_CONNECT_TO_WIFI);
	}else{
		if (canRead && client->available()){
			String msg;
			if (getMessage(msg)){
				size_t jsonSize = msg.length() + JSON_OBJECT_SIZE(10);
				DynamicJsonDocument doc(jsonSize);
				DeserializationError error = deserializeJson(doc, msg);
				if (error) {
					mprint(PSTR("Error to deserialization JSON: %s"), error.c_str());
					return;
				}

				String id        = doc["id"];
				const char* type = doc["type"];

				if (type && strcmp(type, "data") == 0){
					JsonObject data = doc["payload"]["data"];
					String dataString;
        	serializeJson(data, dataString);
					callEvent(id, dataString);
				}
   		}
 
				//callEvent<String>(MWS_INIT_STREAMING, msg);
				//mprint("Handshake: %s \n", msg.c_str());
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
	String payload = PSTR(R"({"type":"connection_init", "payload":{"Authorization": "Test test:test"}})");
	send(payload);
	int timeout = millis() + 10000;
	bool response   = false;
	bool ackSuccess = false;
	while(millis() <= timeout){
		String msg;
		if (getMessage(msg)){
			DynamicJsonDocument doc(256);
			DeserializationError error = deserializeJson(doc, msg);
			response = true;
			if (error){
				mprint(PSTR("Error in convert json response ACK: %s"), error.c_str());
				response = false;
				ackSuccess = false;
			}else{
				const char* type = doc["type"];
				if (strcmp(type, "connection_ack") == 0) {
					//String paydata = R"({"id":"1","type":"start","payload":{"variables":{"serial":"3005EBAE3D98"},"extensions":{},"operationName":null,"query":"subscription {\n  CONTROLE(nr_serie: \"3005EBAE3D98\") {\n    ds_acao\n    parametro {\n      ds_item\n      ds_estado\n    }\n  }\n}\n"}})";
					//send(paydata);
					canRead = true;
					ackSuccess = true;
					startSubscriptions();
				}else{
					ackSuccess = false;
				}
			}
			break;
		}
		delay(100);
	}
	return ackSuccess;
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

void MGraphQL::callAction(const String& actionKey, JsonObject data){
	mprint("\ncallAction~> %s", actionKey.c_str());
	if (this->_action_functions.count(actionKey)){
		this->_action_functions[actionKey](data);
	}else{
		mprint(PSTR("No action %s registered!"), actionKey);
	}
}

void MGraphQL::subscription(String& query, String& variables,  std::function<void(String)> dataJson){
	String id = String(++this->subscriptionId);
	this->subscription(id, query, variables, dataJson);
}

void MGraphQL::subscription(String& id, String& query, String& variables,  std::function<void(String)> dataJson){
	query = formatJson(query);
	variables = formatJson(variables);

	size_t capacity = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(1) + variables.length() + query.length() + 256;
	DynamicJsonDocument doc(capacity);
	doc["id"] = id;
  doc["type"] = "start"; 
	JsonObject payload = doc.createNestedObject("payload");

	String operationName = extractOperationName(query);
	payload["operationName"] = operationName.isEmpty() ? nullptr : operationName.c_str();

	if (variables.isEmpty()){
		payload["variables"] = nullptr;
	}else{
		//capacity = measureJson(variables.c_str()) + 128;
		DynamicJsonDocument docVariables(256);
		DeserializationError error = deserializeJson(docVariables, variables);
		if (error){
			mprint("DeserializationError: %s", error.c_str());
		}else{
			payload["variables"].set(docVariables.as<JsonObject>());
		}
	}

	payload["query"] = query;
	payload.createNestedObject("extensions"); 

	String jsonString;
  serializeJson(doc, jsonString);
	//mprint("ID:%s - %s", id.c_str(), jsonString.c_str());
	this->onEvent<String>(id, [=](String responseJson){
		//mprint(PSTR("-----------> %s"), responseJson.c_str());
		dataJson(responseJson);
	});
	send(jsonString);
}//subscription

void MGraphQL::reconnect() {
	this->canRead = false;
	this->client->stop();
	requestWsUpdate();
}

void MGraphQL::startSubscriptions(){
	this->heartbeatMillis = millis() + 10000;
	subscriptionId = 0;
	String query = PSTR(R"(
		subscription{
			mostrarInformacoesServidor{
				dt_pulso
				nr_versao
			}
	})");

	String variables = "";

	subscription(query, variables, [&](String jsonData){
		this->heartbeatMillis = millis() + 11000;
		mprint("\n~> HB");
		if(!inHeartBeat){
			inHeartBeat = true;
			mprint("\n~> Reconnected to Server\n");
			callEvent(MWS_INIT_SUBSCRIPTIONS);
		}
	});
}

String MGraphQL::formatJson(String jsonString){
  jsonString.replace("\n", " ");
  jsonString.replace("\t", " ");
	while (jsonString.indexOf("  ") != -1) {
			jsonString.replace("  ", " ");
	}
	jsonString.trim();
	return jsonString;
}

String MGraphQL::extractOperationName(const String& query) {
    std::regex regex(PSTR(R"((query|mutation|subscription)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*)"));
    std::cmatch match;

    std::string queryStd = query.c_str();

    if (std::regex_search(queryStd.c_str(), match, regex)) {
        return String(match[2].str().c_str());
    }

    return "";
}