#include <MGraphQL.h>

void MGraphQL::mprint(const char *format, ...){
	if(_cb_mprint){
		va_list args;
		va_start(args, format);
		size_t size = 256; 
		char buffer[size];
		vsnprintf(buffer, size, format, args);
		_cb_mprint(buffer);
		va_end(args);
	}
}