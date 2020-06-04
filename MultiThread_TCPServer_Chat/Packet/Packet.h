#pragma once

//클라이언트 정보
extern struct ClientInfo
{
	ClientInfo(const SOCKET& socket, const SOCKADDR_IN& client_addr, const std::string& client_id)
		: client_sock(socket), client_addr(client_addr), client_id(client_id)
	{

	}
	//클라이언트의 소켓 정보
	SOCKET client_sock;
	//클라이언트 소켓의 주소
	SOCKADDR_IN client_addr;
	//클라이언트 ID
	std::string client_id = "";
};

//패킷 타입
extern enum PacketType
{
	PT_CLIENT_CONNECT     = 1,         // 클라이언트의 연결 성공 시 (클라 -> 서버)
	PT_CLIENT_DISCONNECT  = 2,         // 클라이언트의 연결 해제 성공 시 (클라 -> 서버)
	PT_SERVER_DISACCEPT   = 3,         // 클라이언트의 연결 해제 성공 시 (서버 -> 클라)
	PT_SERVER_ACCEPT      = 4,	       // 클라이언트의 연결 성공 시 (서버 -> 클라)
	PT_CLIENT_SEND        = 5,		   // 클라 -> 서버 (연결요청 외에 보내는 데이터)
	PT_SERVER_SEND		  = 6,         // 서버 -> 클라 (연결요청 외에 보내는 데이터. 즉, 다른 클라가 연결 해제에 성공시 보내는 메시지)
};

//패킷 클래스 구조는 Omok_Multi라는 프로그램을 참고했습니다.
//Packet Header
class PacketHeader
{
public:
	PacketHeader()
	{
		packet_size = 0;
	}

	//생성자 오버로딩
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
//클라이언트가 연결 해제 시 서버가 연결되어 있는 다른 클라이언트들에게 데이터를 전송하고 로그정보를 넘김
class PacketDisAccept : public PacketHeader
{
public:
	PacketDisAccept() {}
	PacketDisAccept(const std::string& id) : PacketHeader(PT_SERVER_DISACCEPT)
	{
		this->client_id = id;
		this->message = client_id + ", disconnected";

		//ServerLog.txt에 저장할 Log 만들기
		//=================================================
		time_t now = time(NULL);

		struct tm date_time;

		date_time = *localtime(&now);

		Log = std::to_string(date_time.tm_year + 1900) + "년도 "
			+ std::to_string(date_time.tm_mon + 1) + "월 "
			+ std::to_string(date_time.tm_mday) + "일 "
			+ std::to_string(date_time.tm_hour) + "시 "
			+ std::to_string(date_time.tm_min) + "분 "
			+ std::to_string(date_time.tm_sec) + "초 "
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
	std::string Log; //로그는 클라이언트에게 보내는 메시지가 아니기 때문에 크기에서 제외
};

//Packet Data(SERVER_ACCEPT) Server -> Client
//현재 연결된 모든 클라이언트에 전송하는 데이터
class PacketAccept : public PacketHeader
{
public:
	PacketAccept() {}
	PacketAccept(const std::string& id) : PacketHeader(PT_SERVER_ACCEPT)
	{
		this->client_id = id;
		this->message = client_id + ", connected";

		//ServerLog.txt에 저장할 Log 만들기
		//=================================================
		time_t now = time(NULL);

		struct tm date_time;

		date_time = *localtime(&now);

		Log = std::to_string(date_time.tm_year + 1900) + "년도 "
			+ std::to_string(date_time.tm_mon + 1) + "월 "
			+ std::to_string(date_time.tm_mday) + "일 "
			+ std::to_string(date_time.tm_hour) + "시 "
			+ std::to_string(date_time.tm_min) + "분 "
			+ std::to_string(date_time.tm_sec) + "초 "
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
	std::string Log; //로그는 클라이언트에게 보내는 메시지가 아니기 때문에 크기에서 제외
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
//어떤 클라이언트가 입력한 메시지를 현재 연결된 모든 클라이언트에 전송하는 데이터
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