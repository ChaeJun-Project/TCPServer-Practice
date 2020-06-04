#pragma once

//변환을 담당하는 클래스
class Convert final
{
public:
	//String -> Char 변환
    static const char * ConvertStringToChar(const std::string& message);
	//Char -> String 변환
	static const std::string ConvertCharToString(const char* message);
};