#include "stdafx.h"
#include "Convert.h"

const char * Convert::ConvertStringToChar(const std::string& message)
{
	char buf[BUFSIZ + 1];

	strcpy(buf, message.c_str());

	return buf;
}

const std::string Convert::ConvertCharToString(const char* message)
{
	std::string str(message);

	return str;
}
