#pragma once

#include "TCPSuper.h"
#include "../Session/Session.h"

class TCPClient final : public TCPSuper
{
public:
	//생성자에서 Initialize 및 Connet 함수 호출
	TCPClient();
	~TCPClient();

	//클라 초기화(WSADATA, SOCKET)
	const bool Initialize() override;

	//Client
	//==================================================================
	//Connect
	bool Connect();

	//클라에서 서버로 데이터 통신
	void CommunicateData();
	//==================================================================

	//클라 구동
	void Run() override;

	//클라 연결 해제
	void Close() override;

	//Set&Get
	//WSADATA
	auto GetWsa() const -> WSADATA  override { return wsa; }

	//Socket
	auto GetSocket() const -> SOCKET  override { return sock; }

	//Socket Address
	auto GetSocketAddr() const -> SOCKADDR_IN  override { return socket_addr; }

	//ServerPort
	auto GetServerPort() const -> int  override { return server_port; }
	void SetServerPort(const int server_port)  override { this->server_port = server_port; }


private:
	const char* server_ip = "127.0.0.1";

	//각 클라이언트는 자신의 정보를 담은 client_info를 스마트포인터로 가짐.
	//Send 및 Recv스레드는 생성될 때 client_info값을 참조하기 때문에 안전하게 관리하기 위해서
	//shared ptr사용
	std::shared_ptr<ClientInfo*> client_info = nullptr;

	//데이터 전송을 위한 스레드
	//3개인 이유는 Connect, DisConnect, Send 총 3가지 패킷 타입이 있기 때문임
	SendThread* send_thread[3];
	//데이터를 받기 위한 스레드
	RecvThread* recv_thread;
};
