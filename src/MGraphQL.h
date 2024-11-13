
#pragma once
#ifndef _MGRAPHQL_H
#define _MGRAPHQL_H
#endif

#include <MWebSocket.h>

#include <cstdio>
#include <functional>
#define PSTR(s)  (s) 
#include <pgmspace.h> 
#include <cstdarg>

typedef enum {
	LED_OFF = 0,
	LED_ON = 1
} io_action_t;

class MGraphQL{
	private:
		//
		//IO actions
		std::function<void(io_action_t action)> _cb_io_action = nullptr;
		//Utils
		std::function<void(char *buffer)> _cb_mprint = nullptr;
		MWebSocket _ws;
		void mprint(const char *formats, ...);
	public:
		void begin(char*url, int port = 443, char *path = "/graphql") {_ws.begin(url, port, path);};
		//IO actions
		void listenerIOEvent(std::function<void(io_action_t action)> action) {this->_cb_io_action = action;};
		//Utils
		void listenerPrintEvent(std::function<void(char *buffer)> _cb_mprint){ this->_cb_mprint = _cb_mprint; };

};