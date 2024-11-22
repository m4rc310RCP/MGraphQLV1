#include "MGraphQL.h"

bool MGraphQL::connectWiFi(){
	//int32_t timeout = millis() + TIMEOUT;
	mprint("Connecting to WiFi %s...", this->_ssid);
	info.millisWiFiConnection = millis() + TIMEOUT;
	bool isConnected = false;
	WiFi.begin(this->_ssid, this->_pass);
	while(millis() <= info.millisWiFiConnection && !isConnected){
		isConnected = WiFi.status() == WL_CONNECTED;
		mprint(".");
		delay(1000);
	}

	if (isConnected){
		info.ip   = WiFi.localIP().toString();
		info.ssid = WiFi.SSID();
		info.rssi = WiFi.RSSI();
		if (info.rssi > -50) {
			info.rssiLevel = "Excellent";
		} else if (info.rssi > -70) {
				info.rssiLevel = "Good";
		} else if (info.rssi > -85) {
				info.rssiLevel = "Weak";
		} else {
				info.rssiLevel = "Poor";
		}

		info.serialNumber = getDeviceSerialNumber();

		mprint("Connected in WiFi SSID: %s IP: %s %sdBm - %s", info.ssid, info.ip, String(info.rssi), info.rssiLevel);
		info.millisWiFiConnection = millis()-1;
		return true;
	}else{
		if (++info.countTryConnection <= 5){
			info.millisWiFiConnection = millis() + TIMEOUT;
			mprint("trying %s of %s...", String(info.countTryConnection), "5");
		}else{
			info.countTryConnection = 0;
			info.millisWiFiConnection = millis() + (10*60*1000);
			mprint("Fail in connection WiFi. Try again in 10 minutes...");
		}
		return false;
	}
}

bool MGraphQL::begin(const char* ssid, const char* pass, const char* host, int port, const char* path){
	this->_ssid = ssid;
	this->_pass = pass;
	this->_host = host;
	this->_path = path;

	info.isStarted = true;

		// client.setInsecure();
		// if(client.connect(host, port)){
		// 	mprint("Requesting upgrade protocol to websocket...");
		// 	char buf[1024];
		// 	sprintf_P(buf, PSTR("GET %s HTTP/1.1\r\n"
		// 										"Host: %s\r\n"
		// 										"Upgrade: websocket\r\n"
		// 										"Connection: Upgrade\r\n"
		// 										"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		// 										"Origin: https://%s\r\n"
		// 										"Sec-WebSocket-Protocol: graphql-ws\r\n"
		// 										"Sec-WebSocket-Version: 13\r\n\r\n"),
		// 	path, host, host);

		// 	bool wsupdated = false;
		// 	bool endOfResponse = false;
		// 	String s;
		// 	client.write((uint8_t*)buf, strlen(buf)); 
		// 	while (!endOfResponse && (s = client.readStringUntil('\n')).length() > 0) {
		// 		if (s.indexOf("HTTP/") != -1) {
		// 			auto status = s.substring(9, 12);
		// 			if (status == "101"){
		// 				wsupdated = true;
		// 			}else {
		// 				wsupdated = false;
		// 			}
		// 		} else if (s == "\r"){
		// 			endOfResponse = true;
		// 		}
		// 	};
		// 	if (wsupdated){
		// 		mprint("Websocket upgrade headers success!");
		// 	}
		// }else{
		// 	mprint("Websocket upgrade headers ERROR!");
		// }
	//client.setInsecure();
	
	// onEvent<ConnectionInfo>(MWS_CONNECT_WIFI_OK, [&](ConnectionInfo info){
	// 	mprint("Connected in WiFi %s with IP: %s Signal Level: %sdBm (%s)",
	// 		info.ssid, info.ip, String(info.rssi), info.rssiLevel
	// 	);

	// });

	//mprint("Connecting in WiFi...");
	//callEvent<int32_t>(MWS_CONNECT_WIFI, millis() + 5000);


	// if (client.connect(host, port)){
	// 	mprint("Try upgrade for websocket protocol...");
	// }
	return true;
}

void MGraphQL::loop(void){
	if(info.isStarted){
		int32_t currentMillis = millis();
		// if (currentMillis >= info.millisWiFiConnection && WiFi.status() != WL_CONNECTED){
		// 	connectWiFi();
		// }else 
		if (currentMillis >= info.millisServerConnection && !client.connected()){
			//upgradeToWS();
		}
	}
}

void MGraphQL::upgradeToWS(const char * host, int port, const char * path){
	info.millisServerConnection = millis() + TIMEOUT;
	client.setInsecure();
	mprint("Connecting for %s...", host);
	if(client.connect(host, port)){
		char buf[1024];
		sprintf_P(buf, PSTR("GET %s HTTP/1.1\r\n"
										"Host: %s\r\n"
										"Upgrade: websocket\r\n"
										"Connection: Upgrade\r\n"
										"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
										"Origin: https://%s\r\n"
										"Sec-WebSocket-Protocol: graphql-ws\r\n"
										"Sec-WebSocket-Version: 13\r\n\r\n"),
		path, host, host);
		mprint("Upgrading for WebSocket...");

		bool endOfResponse = false;
		bool wsupdated     = false;
		String s;
		client.write((uint8_t*)buf, strlen(buf)); 
	 	while (!endOfResponse && (s = client.readStringUntil('\n')).length() > 0) {
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
		};

		if (wsupdated){
			mprint("Handshack...");
			handshake();
		}
	}

}

String MGraphQL::getDeviceSerialNumber(){
	uint64_t chipId = ESP.getEfuseMac();
	char serialNumber[18];
  snprintf(serialNumber, sizeof(serialNumber), "%04X%08X",(uint16_t)(chipId >> 32), (uint32_t)chipId);
	return String(serialNumber);
}

bool MGraphQL::handshake(){
	String payload = R"({"type":"connection_init", "payload":{"Authorization": "Test test:test"}})";
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

		payload = R"({"type":"ping", "payload":{}})";
		send(payload);
		String msg2;
		if (getMessage(msg2)){
			mprint(">> %s", msg2.c_str());
		}
		delay(100);
	}
	return true;
}


void MGraphQL::mprint(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    callEvent(MWS_STREAMING_MESSAGE, buffer);
}

void MGraphQL::send(const String& str){
	client.write(WS_FIN | WS_OPCODE_TEXT);
	int size = str.length();
	if (size > 125) {
		client.write(WS_MASK | WS_SIZE16);
		client.write((uint8_t) (size >> 8));
		client.write((uint8_t) (size & 0xFF));
	} else {
		client.write(WS_MASK | (uint8_t) size);
	}

	// 3. send mask
	uint8_t mask[4];
	mask[0] = random(0, 256);
	mask[1] = random(0, 256);
	mask[2] = random(0, 256);
	mask[3] = random(0, 256);

	client.write(mask[0]);
	client.write(mask[1]);
	client.write(mask[2]);
	client.write(mask[3]);

	//4. send masked data
	for (int i = 0; i < size; ++i) {
		client.write(str[i] ^ mask[i % 4]);
	}
}

bool MGraphQL::getMessage(String& message){
	if (!client.connected()) {	return false; }
	if(!client.available()){
		return false;
	}
	// 1. read type and fin
	unsigned int msgtype = timedRead();
	if (!client.connected()) {return false;}

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
	while (!client.available()) {
		delay(20);
	}
	return client.read();
}

