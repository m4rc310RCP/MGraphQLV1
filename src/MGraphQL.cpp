#include "MGraphQL.h"

bool MGraphQL::begin(char* host, int port, char* path){
		client.setInsecure();
		if(client.connect(host, port)){
			mprint("Requesting upgrade protocol to websocket...");
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

			bool wsupdated = false;
			bool endOfResponse = false;
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
				mprint("Websocket upgrade headers success!");
			}
		}else{
			mprint("Websocket upgrade headers ERROR!");
		}
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

void MGraphQL::mprint(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    callEvent(MWS_STREAMING_MESSAGE, buffer);
}

