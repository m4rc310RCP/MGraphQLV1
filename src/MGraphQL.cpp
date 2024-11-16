
#include "MGraphQL.h"

void MGraphQL::addPrintListener(const std::function<void(const std::string&)>& callback) {
    _callback = callback;
}

void MGraphQL::mprint(const char* format, ...) {
    if (_callback) {
        char buffer[256];

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        // Converte o buffer para std::string e chama o callback
        _callback(std::string(buffer));
    }
}