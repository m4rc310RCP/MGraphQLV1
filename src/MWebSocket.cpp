#include "MGraphQL.h"

MUtils utils;

void MWebSocket::begin(char* host, int port, char* path){
	this->_host = host;
	this->_port = port;
	this->_path = path;
	utils.mprint("----> %s", host);
}

