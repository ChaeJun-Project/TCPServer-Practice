#pragma once

//��ȯ�� ����ϴ� Ŭ����
class Convert final
{
public:
	//String -> Char ��ȯ
    static const char * ConvertStringToChar(const std::string& message);
	//Char -> String ��ȯ
	static const std::string ConvertCharToString(const char* message);
};