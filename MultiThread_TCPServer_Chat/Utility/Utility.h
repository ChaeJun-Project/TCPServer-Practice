#pragma once

//받은 메시지로 부터 원하는 데이터를 얻기위해 도와주는 클래스
class Utility final
{
public:
	//받은 메시지로부터 client의 ID를 얻음
	static const std::string GetClientIDFromData(const char* buf);
	//받은 메시지로부터 client의 ID를 얻음
	static const std::string GetClientIDFromDataWithPacketType(const char* buf);
	//받은 데이터로부터 메시지를 얻음
	static const std::string GetMessageFromData(const char* buf);
	//패킷 유형이 포함된 데이터를 받을 때 패킷 유형을 얻음(1, 5만 필요)
	static const int GetPacketTypeFromData(const char* buf);
	//패킷 유형이 포함된 데이터를 받을 때 패킷 유형을 제외하고 얻음(1, 5만 필요)
	static const std::string GetWithoutPacketTypeFromData(const char* buf);
};