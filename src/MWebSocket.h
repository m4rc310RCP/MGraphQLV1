#pragma once
#include <MGraphQL.h>

#ifndef _MWEBSOCKET_H
#define _MWEBSOCKET_H
#endif


typedef enum {
	MWS_CONNECTING = 0,
	MWS_CONNECTED = 1,
	MWS_ERROR = 2,
	MWS_FATAL = 3
} ws_status_t;

class MWebSocket {
private:
		//WS
	char *_url;
	int  _port;
	char *_path;
	ws_status_t _status;
public:
	void begin(char*url, int port, char *path);
	ws_status_t status() { return _status; };
};
