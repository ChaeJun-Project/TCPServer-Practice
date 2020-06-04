#pragma once

#define SERVERPORT 9000
#define BUFSIZE    512

class Session;

class TCPSuper
{
public:
	TCPSuper() = default;
	virtual ~TCPSuper() = default;

	//서버 or 클라 초기화(WSADATA, SOCKET)
	virtual const bool Initialize() = 0;

	//서버 or 클라 구동
	virtual void Run() = 0;

	//서버 or 클라 해제
	virtual void Close() = 0;

	//Set&Get
	//WSADATA
	virtual auto GetWsa() const -> WSADATA = 0;

	//Socket
	virtual auto GetSocket() const -> SOCKET = 0;

	//Socket Address
	virtual auto GetSocketAddr() const -> SOCKADDR_IN = 0;

	//ServerPort
	virtual auto GetServerPort() const -> int = 0;
	virtual void SetServerPort(const int server_port) = 0;

protected:
	//윈속 초기화 정보 구조체
	WSADATA wsa;
	//소켓
	SOCKET sock;
	//서버 소켓의 주소
	SOCKADDR_IN socket_addr;
	//Client ID
	std::string client_id = "Client";

	//서버포트
	int server_port = SERVERPORT;
	//데이터를 Recv하거나 Send할 때 오류체크를 위한 변수
	int retval = 0;
	//데이터가 정상적으로 send되었는지 확인할 변수
	bool result = false;
};