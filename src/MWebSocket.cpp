#include <MGraphQL.h>
#include <MWebSocket.h>


void MWebSocket::begin(char*url, int port, char *path){
	this->_url = url;
	this->_port = port;
	this->_path = path;
	
	mprint(PSTR("MMMMMM \n"));
	// mprint(PSTR("MMMMMM \n"));
}