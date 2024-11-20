#include "MGraphQL.h"

bool MGraphQL::begin(char* host, int port, char* path){
	this->_host = host;
	this->_port = port;
	this->_path = path;
	//_callback_ws_event(MWS_CONNECT_WIFI);

	if (client.connect(host, port)){
		mprint("....");
	}
	
	

	return true;
}

void MGraphQL::setSerialListener(std::function<void(char *message)> callback){ 
	_callback_serial_print = callback;
};

void MGraphQL::mprint(const char *format, ...) {
	  if (_callback_serial_print) {
				va_list args;
				va_start(args, format);
				size_t size = 256; 
				char buffer[size];
				vsnprintf(buffer, size, format, args);
				_callback_serial_print(buffer);
				va_end(args);
    } else {
        Serial.println("** Callback não definido **");
    }
}

