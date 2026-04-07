#pragma once

#include <sstream>
#include <stdexcept>
#include <windows.h>
#include <iostream>

std::string GetHRESULTErrorMessage(HRESULT hr)
{
	char* errorMsg = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&errorMsg,
		0,
		nullptr
	);

	std::string message = errorMsg ? errorMsg : "Unknown error";
	LocalFree(errorMsg);
	return message;
}

#define ERROR_CHECK( ret )																	\
    if( FAILED( ret ) ){																	\
        std::stringstream ss;																\
        ss << "failed " #ret " " << std::hex << ret << std::endl;							\
        std::cerr << "Error: " << GetHRESULTErrorMessage( ret ) << std::endl;               \
        throw std::runtime_error( ss.str().c_str() );										\
    }