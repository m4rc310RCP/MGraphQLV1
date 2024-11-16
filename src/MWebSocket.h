#pragma once

#ifndef MWEBSOCKET_H
#define MWEBSOCKET_H
#endif

#include "MUtils.h"

class MWebSocket {
	private:
		char* _host;
		int   _port;
		char* _path;
		MUtils _utils;
	public:
		std::function<void(const std::string&)> _callback = _utils._callback;
		void begin(char* host, int port, char* path);
};
