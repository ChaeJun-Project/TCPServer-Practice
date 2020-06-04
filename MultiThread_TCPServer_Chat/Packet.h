#pragma once

//Ŭ���̾�Ʈ ����
extern struct ClientInfo
{
	ClientInfo(const SOCKET& socket, const SOCKADDR_IN& client_addr, const std::string& client_id)
		: client_sock(socket), client_addr(client_addr), client_id(client_id)
	{

	}
	//Ŭ���̾�Ʈ�� ���� ����
	SOCKET client_sock;
	//Ŭ���̾�Ʈ ������ �ּ�
	SOCKADDR_IN client_addr;
	//Ŭ���̾�Ʈ ID
	std::string client_id = "";
};

//��Ŷ Ÿ��
extern enum PacketType
{
	PT_CLIENT_CONNECT     = 1,         // Ŭ���̾�Ʈ�� ���� ���� �� (Ŭ�� -> ����)
	PT_CLIENT_DISCONNECT  = 2,         // Ŭ���̾�Ʈ�� ���� ���� ���� �� (Ŭ�� -> ����)
	PT_SERVER_DISACCEPT   = 3,         // Ŭ���̾�Ʈ�� ���� ���� ���� �� (���� -> Ŭ��)
	PT_SERVER_ACCEPT      = 4,	       // Ŭ���̾�Ʈ�� ���� ���� �� (���� -> Ŭ��)
	PT_CLIENT_SEND        = 5,		   // Ŭ�� -> ���� (�����û �ܿ� ������ ������)
	PT_SERVER_SEND		  = 6,         // ���� -> Ŭ�� (�����û �ܿ� ������ ������. ��, �ٸ� Ŭ�� ���� ������ ������ ������ �޽���)
};

//��Ŷ Ŭ���� ������ Omok_Multi��� ���α׷��� �����߽��ϴ�.
//Packet Header
class PacketHeader
{
public:
	PacketHeader()
	{
		packet_size = 0;
	}

	//������ �����ε�
	PacketHeader(PacketType type)
	{
		packet_type = type;
		packet_size = 0;
	}

	PacketType GetPacketType() const { return packet_type; }
	unsigned int GetPacketSize() const { return packet_size; }
	virtual std::string GetClientID() const = 0;
	virtual std::string GetDataMessage() const = 0;

protected:
	virtual void CalcPacketSize() = 0;

	PacketType packet_type;
	unsigned int packet_size;
};

//Packet Data(CLIENT_CONNECT) Client -> Server
class PacketConnect : public PacketHeader
{
public:
	PacketConnect() {}
	PacketConnect(const std::string& id) : PacketHeader(PT_CLIENT_CONNECT)
	{
		client_id = id;
		message =  client_id + ", connection request success";

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
};

//Packet Data(CLIENT_DISCONNECT) Client -> Server
class PacketDisConnect : public PacketHeader
{
public:
	PacketDisConnect() {}
	PacketDisConnect(const std::string& id) : PacketHeader(PT_CLIENT_DISCONNECT)
	{
		client_id = id;
		message = "2!" + client_id + ", disconnection request success";

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
};

//Packet Data(CLIENT_DISCONNECT) Server -> Client
//Ŭ���̾�Ʈ�� ���� ���� �� ������ ����Ǿ� �ִ� �ٸ� Ŭ���̾�Ʈ�鿡�� �����͸� �����ϰ� �α������� �ѱ�
class PacketDisAccept : public PacketHeader
{
public:
	PacketDisAccept() {}
	PacketDisAccept(const std::string& id) : PacketHeader(PT_SERVER_DISACCEPT)
	{
		this->client_id = id;
		this->message = client_id + ", disconnected";

		//ServerLog.txt�� ������ Log �����
		//=================================================
		time_t now = time(NULL);

		struct tm date_time;

		date_time = *localtime(&now);

		Log = std::to_string(date_time.tm_year + 1900) + "�⵵ "
			+ std::to_string(date_time.tm_mon + 1) + "�� "
			+ std::to_string(date_time.tm_mday) + "�� "
			+ std::to_string(date_time.tm_hour) + "�� "
			+ std::to_string(date_time.tm_min) + "�� "
			+ std::to_string(date_time.tm_sec) + "�� "
			+ message;
		//=================================================

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }
	std::string GetLogStr() const { return Log; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
	std::string Log; //�α״� Ŭ���̾�Ʈ���� ������ �޽����� �ƴϱ� ������ ũ�⿡�� ����
};

//Packet Data(SERVER_ACCEPT) Server -> Client
//���� ����� ��� Ŭ���̾�Ʈ�� �����ϴ� ������
class PacketAccept : public PacketHeader
{
public:
	PacketAccept() {}
	PacketAccept(const std::string& id) : PacketHeader(PT_SERVER_ACCEPT)
	{
		this->client_id = id;
		this->message = client_id + ", connected";

		//ServerLog.txt�� ������ Log �����
		//=================================================
		time_t now = time(NULL);

		struct tm date_time;

		date_time = *localtime(&now);

		Log = std::to_string(date_time.tm_year + 1900) + "�⵵ "
			+ std::to_string(date_time.tm_mon + 1) + "�� "
			+ std::to_string(date_time.tm_mday) + "�� "
			+ std::to_string(date_time.tm_hour) + "�� "
			+ std::to_string(date_time.tm_min) + "�� "
			+ std::to_string(date_time.tm_sec) + "�� "
			+ message;
		//=================================================

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }
	std::string GetLogStr() const { return Log; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
	std::string Log; //�α״� Ŭ���̾�Ʈ���� ������ �޽����� �ƴϱ� ������ ũ�⿡�� ����
};

//Packet Data(CLIENT_SEND) Client -> Server
class PacketClientSend : public PacketHeader
{
public:
	PacketClientSend() {}
	PacketClientSend(const std::string& id, const std::string& message) : PacketHeader(PT_CLIENT_SEND)
	{
		this->client_id = id;
		this->message = "5!" + client_id + "," + message;

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
};

//Packet Data(SERVER_SEND) Server -> Client
//� Ŭ���̾�Ʈ�� �Է��� �޽����� ���� ����� ��� Ŭ���̾�Ʈ�� �����ϴ� ������
class PacketServerSend : public PacketHeader
{
public:
	PacketServerSend() {}
	PacketServerSend(const std::string& id, const std::string& client_port, const std::string& message) : PacketHeader(PT_SERVER_SEND)
	{
		this->client_id = id;
		this->message = "[" + client_id + "(" + client_port + ")" + "]: " + message;

		CalcPacketSize();
	}

	std::string GetClientID() const { return client_id; }
	std::string GetDataMessage() const { return message; }

private:
	void CalcPacketSize()
	{
		packet_size = message.size();
	}

	std::string client_id;
	std::string message;
};