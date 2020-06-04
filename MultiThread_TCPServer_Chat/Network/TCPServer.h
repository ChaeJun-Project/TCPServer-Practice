#pragma once

#include "TCPSuper.h"

class TCPServer final : public TCPSuper
{
public:
	TCPServer();
	~TCPServer();

	//서버 초기화(WSADATA, SOCKET)
	const bool Initialize() override;

	//Server
	//==================================================================
	//Bind
	bool Bind();

	//listen
	bool Listen();

	//Accept
	void Accept();

	//Create Client ID
	void CreateClientID();

	//서버 -> 클라로 접속 정보를 넘김
	void SendAccept(const std::string& client_id);

	//서버 -> 클라로 특정 클라 접속 해제 정보를 넘김
	void SendDisAccept(const std::string& client_id);

	//Write ServerLog.txt
	void WriteFile(const std::string& log);

	//서버 -> 클라로 데이터를 넘김
	void SendData(const char * buf);

	//서버에서 클라이언트로 데이터 통신, Accept함수에서 스레드로 병렬처리
	void CommunicateData(const std::string& client_id);

	//접속을 해제한 클라이언트의 주소와 포트번호를 출력
	void PrintIPAndPort(const SOCKADDR_IN client_addr);
	//==================================================================

	//서버 구동
	void Run() override;

	//서버 연결 해제
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
	//Client의 정보를 저장
	void AddClientInfo(ClientInfo* client_info);

	//저장된 Client의 정보를 Client_id를 통해 삭제
	void DeleteClientInfo(const std::string& client_id);

	//저장된 Client의 정보를 Client_id를 통해 반환
	ClientInfo* GetClientInfo(const std::string& client_id) const;

	//데이터 통신에 사용할 변수
	//처음에 Accept에 성공한 클라이언트의 소켓값을 저장할 변수
	SOCKET client_sock;
	//Accept에 성공한 클라이언트의 소켓 주소값을 저장할 변수
	SOCKADDR_IN client_addr;
	//클라이언트의 개수
	int client_num = 0;
	int addrlen;

	//연결된 클라이언트의 정보를 저장할 vector
	std::vector<ClientInfo*> client_infos;

	//서버로그 텍스트 파일
	std::string server_log = "ServerLog.txt";

	//받은 데이터를 저장할 버퍼
	char buf[BUFSIZE];

	//동기화를 위한 mutex
	std::mutex mutex;
};