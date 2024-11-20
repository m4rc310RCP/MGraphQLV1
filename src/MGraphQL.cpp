#include "MGraphQL.h"

bool MGraphQL::begin(char* host, int port, char* path){
	this->_host = host;
	this->_port = port;
	this->_path = path;
	// --------------------------------------------------------------------------------
	mprint("\nStartup application ESP32 S/N: %s\n", getDeviceSerialNumber());
	// --------------------------------------------------------------------------------
	
	return true;
}

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

String MGraphQL::getDeviceSerialNumber(){
	uint64_t chipId = ESP.getEfuseMac();
	char serialNumber[18];
  snprintf(serialNumber, sizeof(serialNumber), "%04X%08X",(uint16_t)(chipId >> 32), (uint32_t)chipId);
	return String(serialNumber);
}

