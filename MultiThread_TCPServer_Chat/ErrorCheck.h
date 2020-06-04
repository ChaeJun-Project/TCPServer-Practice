#pragma once

//�̱��� ����
class ErrorCheck final
{
public:
	static ErrorCheck& Get()
	{
		static ErrorCheck instance;
		return instance;
	}

    ErrorCheck() = default;
	~ErrorCheck() = default;

	//���� �Լ� ���� ��� �� ����
	void err_quit(const char *msg);
	//���� �Լ� ���� ���
	void err_display(const char *msg);
};