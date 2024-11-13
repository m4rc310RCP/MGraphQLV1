#pragma once

#include <MWebSocket.h>
#include <MUtils.h>

#include <cstdio>
#include <functional>
#define PSTR(s)  (s) 
#include <pgmspace.h> 
#include <cstdarg>
#include <string>
#include <iostream>

class MGraphQL{
	private:
		MWebSocket _ws;
		MUtils _utils;
	public:
		// void mprint(const char *format, ...) { this->_utils.mprint;};
		void addPrintListener(const std::function<void(const std::string&)>& callback){ this->_utils._callback = callback; };
		void begin(char* host, int port = 443, char* path = "/grapql") { _ws.begin(host, port, path); };
};
