#pragma once
#include <string>  
#include <functional>

#include "MGraphQL.h"
#ifndef MUTILS_H
#define MUTILS_H
#endif

class MUtils{
private:
public:
	std::function<void(const std::string&)> _callback; 
	void addPrintListener(const std::function<void(const std::string&)>& callback);
	void mprint(const char* format, ...); 
		
};
