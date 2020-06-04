#pragma once

//���� �޽����� ���� ���ϴ� �����͸� ������� �����ִ� Ŭ����
class Utility final
{
public:
	//���� �޽����κ��� client�� ID�� ����
	static const std::string GetClientIDFromData(const char* buf);
	//���� �޽����κ��� client�� ID�� ����
	static const std::string GetClientIDFromDataWithPacketType(const char* buf);
	//���� �����ͷκ��� �޽����� ����
	static const std::string GetMessageFromData(const char* buf);
	//��Ŷ ������ ���Ե� �����͸� ���� �� ��Ŷ ������ ����(1, 5�� �ʿ�)
	static const int GetPacketTypeFromData(const char* buf);
	//��Ŷ ������ ���Ե� �����͸� ���� �� ��Ŷ ������ �����ϰ� ����(1, 5�� �ʿ�)
	static const std::string GetWithoutPacketTypeFromData(const char* buf);
};