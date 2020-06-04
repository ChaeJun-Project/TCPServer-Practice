#include "stdafx.h"
#include "TCPClient.h"

TCPClient::TCPClient()
{
	//Initialize
	assert(Initialize());
	//Connect
	assert(Connect());
}

TCPClient::~TCPClient()
{
	//받기 전용 스레드 해제
	recv_thread->CloseThread();
	//만약 해제가 안 된 스레드가 있다면 종료시킴
	for (int i = 0; i < 3; ++i)
	{
	    //전송 전용 스레드 해제
		send_thread[i]->CloseThread();
	}
	Close();
}

const bool TCPClient::Initialize()
{
	//윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	//소켓 초기화
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		ErrorCheck::Get().err_quit("socket()");
		return false;
	}

	//socket_addr 초기화
	ZeroMemory(&socket_addr, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = inet_addr(server_ip);
	socket_addr.sin_port = htons(server_port);

	return true;
}

bool TCPClient::Connect()
{
	char buf[BUFSIZE];

	//buf의 내용을 다 지움
	ZeroMemory(buf, sizeof(buf));

	retval = connect(sock, (SOCKADDR *)&socket_addr, sizeof(socket_addr));
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_quit("bind()");
		return false;
	}

	//Client ID를 받음
	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_display("recv()");
		return false;
	}

	//Client ID를 저장
	client_id = Utility::GetClientIDFromData(buf);

	//Client ID출력
	std::cout << "[" << client_id << "(" + Utility::GetMessageFromData(buf) + ")" + "]" << std::endl;

	//Shared Ptr ClientInfo생성
	client_info = std::make_shared<ClientInfo*>(new ClientInfo(sock, socket_addr, client_id));

	//클라이언트가 서버에 Connect 성공을 하면 미리 전송에 사용할 스레드들을 동적할당함
	send_thread[0] = new SendThread(*client_info, PT_CLIENT_CONNECT);
	send_thread[1] = new SendThread(*client_info, PT_CLIENT_SEND);
	send_thread[2] = new SendThread(*client_info, PT_CLIENT_DISCONNECT);
	recv_thread = new RecvThread(*client_info);

	//Connet 성공
	//Send ClientConnectPacket
	send_thread[0]->StartThread();
	send_thread[0]->JoinThread();
	send_thread[0]->CloseThread();

	return true;
}

void TCPClient::CommunicateData()
{
	while (1)
	{
	    //전송 스레드2 시작
		send_thread[1]->StartThread();
		//받기 스레드 시작
		recv_thread->StartThread();
		//클라이언트가 "/bye를" 데이터로 넘긴 경우
		//while문을 빠져나감
		if (send_thread[1]->IsExitClient())
		{
			break;
		}
	}
	//전송 스레드2 해제
	send_thread[1]->CloseThread();
	//받기 스레드 해제
	recv_thread->CloseThread();
	//전송 스레드3 시작
	send_thread[2]->StartThread();
	//전송 스레드3이 종료될 때까지 기다림
	send_thread[2]->JoinThread();
	//전송 스레드3 스레드 해제
	send_thread[2]->CloseThread();
	//소켓 연결해제
	Close();
}

void TCPClient::Run()
{
	CommunicateData();
}

void TCPClient::Close()
{
	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
}
