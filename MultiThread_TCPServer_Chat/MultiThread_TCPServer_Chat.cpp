#include "stdafx.h"
#include "TCPServer.h"
#include "TCPClient.h"

int main(int argc, char *argv[])
{
	//서버
	//==============================================================
	TCPServer *server = new TCPServer();
	
	//서버작동
	server->Run();
	
	//소멸자 호출 후 소켓 및 윈속 종료
	delete server;
	//==============================================================

	//클라이언트
	//==============================================================
	//TCPClient *client = new TCPClient();
	//
	////서버작동
	//client->Run();
	//
	////소멸자 호출 후 소켓 및 윈속 종료
	//delete client;
	//==============================================================

	return 0;
}