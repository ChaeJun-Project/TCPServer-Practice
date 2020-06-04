#include "stdafx.h"
#include "Utility.h"

const std::string Utility::GetClientIDFromData(const char * buf)
{
	std::string str(Convert::ConvertCharToString(buf));

	int last_index = str.find_last_of(',');

	std::string client_id = str.substr(0, last_index);

	return client_id;
}

const std::string Utility::GetClientIDFromDataWithPacketType(const char * buf)
{
	std::string str(GetClientIDFromData(buf));

	int first_index = str.find_first_of('!');

	std::string client_id = str.substr(first_index + 1, str.size());

	return client_id;
}

const std::string Utility::GetMessageFromData(const char * buf)
{
	std::string str(Convert::ConvertCharToString(buf));

	int first_index = str.find_first_of(',');

	std::string message = str.substr(first_index + 1, str.size());

	return message;
}

const int Utility::GetPacketTypeFromData(const char * buf)
{
	std::string str(Convert::ConvertCharToString(buf));

	int last_index = str.find_last_of('!');

	std::string type = str.substr(0, last_index);

	return std::stoi(type);
}

const std::string Utility::GetWithoutPacketTypeFromData(const char * buf)
{
	std::string str(Convert::ConvertCharToString(buf));

	int first_index = str.find_last_of('!');

	std::string without_type = str.substr(first_index + 1, str.size());

	return without_type;
}
