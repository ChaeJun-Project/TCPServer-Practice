#include "stdafx.h"
#include "TCPServer.h"

TCPServer::TCPServer()
{
	//Initialize
	assert(Initialize());
	std::cout << "[B477040 채준 TCP_Chat_Server]" << std::endl;
}

TCPServer::~TCPServer()
{
    //client_infos벡터에 있는 모든 데이터 메모리 해제
	for (int i = 0; i < client_infos.size(); ++i)
	{
		delete client_infos[i];
	}
	Close();
}

const bool TCPServer::Initialize()
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
	socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_addr.sin_port = htons(server_port);

	//Bind
	assert(Bind());

	//Listen
	assert(Listen());

	return true;
}

bool TCPServer::Bind()
{
	//bind()
	retval = bind(sock, (SOCKADDR *)&socket_addr, sizeof(socket_addr));
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_quit("bind()");
		return false;
	}

	return true;
}

bool TCPServer::Listen()
{
	//listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		ErrorCheck::Get().err_quit("listen()");
		return false;
	}

	return true;
}

void TCPServer::Accept()
{
	while (1)
	{
		//accept()
		addrlen = sizeof(client_addr);
		client_sock = accept(sock, (SOCKADDR *)&client_addr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			break;
		}

		//buf의 내용을 다 지움
		ZeroMemory(buf, sizeof(buf));

		//Create Client ID
		CreateClientID();

		//버퍼에 Client ID를 저장
		strcpy(buf, Convert::ConvertStringToChar(client_id));

		//Client ID 초기화
		client_id = "Client";

		//client id와 포트번호를 구분하기 위함
		strcat(buf, ",");
		//버퍼에 Client Port번호도 복사
		strcat(buf, Convert::ConvertStringToChar(std::to_string(client_addr.sin_port)));

		//연결된 클라이언트에게 Client ID를 보냄
		retval = send(client_sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			return;
		}

		//PacketConnect 패킷을 받음
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			return;
		}
		else if (retval == 0)
			return;

		//PacketConnect 패킷의 데이터를 출력
		std::cout << buf << std::endl;

		//buf의 내용 중 연결에 성공한 Client ID를 추출
		std::string client_id = Utility::GetClientIDFromData(buf);

		//새로 연결된 클라이언트의 정보를 session에 있는 client_infos 벡터에 추가
		AddClientInfo(new ClientInfo(client_sock, client_addr, client_id));

		//Send ClientAcceptPacket
		SendAccept(client_id);

		//스레드 풀에 this포인터와 위에서 구한 client_id를 캡처하고 CommunicateData()함수를 풀에 등록
		ThreadPool::Get()->EnqueueJob([this, client_id]() { this->CommunicateData(client_id); } );
	}
}

void TCPServer::AddClientInfo(ClientInfo * client_info)
{
	client_infos.emplace_back(client_info);
}

void TCPServer::DeleteClientInfo(const std::string & client_id)
{
	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id == client_id)
		{
			delete client_infos[i];
			client_infos.erase(client_infos.begin() + i);
		}
	}
}

ClientInfo * TCPServer::GetClientInfo(const std::string & client_id) const
{
	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id == client_id)
		{
			return client_infos[i];
		}
	}
}

void TCPServer::CreateClientID()
{
	client_num++;
	client_id = client_id + std::to_string(client_num);
}

void TCPServer::SendAccept(const std::string& client_id)
{
	//buf의 내용을 다 지움
	ZeroMemory(buf, sizeof(buf));

	PacketHeader *packet = new PacketAccept(client_id);

	strcpy(buf, Convert::ConvertStringToChar(packet->GetDataMessage()));

	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id != client_id)
		{
			retval = send(client_infos[i]->client_sock, buf, sizeof(buf), 0);
			assert(retval);
		}
	}

	//다운 캐스팅
	PacketAccept *packet_accept = dynamic_cast<PacketAccept*>(packet);

	//ServerLog.txt 파일에 접속 종료한 ID를 등록
	WriteFile(packet_accept->GetLogStr());

	delete packet;
}

void TCPServer::SendDisAccept(const std::string & client_id)
{
	//buf의 내용을 다 지움
	ZeroMemory(buf, sizeof(buf));

	PacketHeader *packet = new PacketDisAccept(client_id);

	strcpy(buf, Convert::ConvertStringToChar(packet->GetDataMessage()));

	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id != client_id)
		{
			retval = send(client_infos[i]->client_sock, buf, sizeof(buf), 0);
			assert(retval);
		}
	}

	//다운 캐스팅
	PacketDisAccept *packet_disaccept = dynamic_cast<PacketDisAccept*>(packet);

	//ServerLog.txt 파일에 접속 종료한 ID를 등록
	WriteFile(packet_disaccept->GetLogStr());

	

	delete packet;
}

void TCPServer::WriteFile(const std::string& log)
{
    //log내용 출력
	std::cout << log << std::endl;

	std::ofstream write_file;
	//std::fstream::app은 기존의 파일에 내용을 더 추가해서 작성하는 옵션임
	write_file.open(server_log, std::fstream::app);

	if (write_file.is_open())
	{
		write_file << log;

		//줄바꿈
		write_file << std::endl;
	}

	write_file.close();
}

void TCPServer::SendData(const char * buf)
{
	char buffer[BUFSIZE];

	//패킷 유형이 데이터에 포함된 경우 데이터에서 client_id만 빼냄 
	std::string client_id = Utility::GetClientIDFromDataWithPacketType(buf);

	//Client_ID가 포함되어 있는 데이터에서 실제 다른 클라로 보낼 데이터만 빼냄
	std::string message = Utility::GetMessageFromData(buf);

	//PacketServerSend패킷 동적할당
	PacketHeader *packet = new PacketServerSend(client_id, std::to_string(GetClientInfo(client_id)->client_addr.sin_port), message);

	//데이터를 보낼 버퍼에 생성한 패킷의 데이터를 넘김
	strcpy(buffer, Convert::ConvertStringToChar(packet->GetDataMessage()));

	//client_infos에 저장된 값들을 모두 순회하면서
	//해당 client_id가 위에서 구한 id와 다를 경우만 데이터를 전송
	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id != client_id)
		{
			retval = send(client_infos[i]->client_sock, buffer, sizeof(buffer), 0);
			assert(retval);
		}
	}

	//동적할당한 패킷 해제
	delete packet;
}

void TCPServer::CommunicateData(const std::string& client_id)
{
	// 클라이언트 정보 얻기
	SOCKET client_sock = GetClientInfo(client_id)->client_sock;
	SOCKADDR_IN client_addr = GetClientInfo(client_id)->client_addr;
	
	//buf의 내용을 다 지움
	ZeroMemory(buf, sizeof(buf));

	// 클라이언트와 데이터 통신
	while (1)
	{
		//buf의 내용을 다 지움
		ZeroMemory(buf, sizeof(buf));

		//데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			break;
		}
		else if (retval == 0)
			break;

	    //클라이언트가 정상적으로 접속을 종료했을 경우
		if (Utility::GetPacketTypeFromData(buf) == static_cast<int>(PT_CLIENT_DISCONNECT))
		{
			//접속을 해제한 클라이언트의 주소와 포트번호를 출력
			PrintIPAndPort(client_addr);
		    std::cout<< Utility::GetWithoutPacketTypeFromData(buf) <<std::endl;
		    break;
		}

		//클라이언트가 정상적으로 메시지를 보냈을 경우
		if (Utility::GetPacketTypeFromData(buf) == static_cast<int>(PT_CLIENT_SEND))
		{
			//현재 클라이언트를 제외한 나머지 클라들에게 메시지 전달
			SendData(buf);
		}

	}

	//강제로 연결 종료
	if (retval == SOCKET_ERROR)
	{
		//접속을 해제한 클라이언트의 주소와 포트번호를 출력
		PrintIPAndPort(client_addr);
		std::cout << client_id <<", disconnection request success" << std::endl;
	}

	//PacketDisAccept 패킷을 아직 연결 중인 클라이언트로 전송
	SendDisAccept(client_id);

	//뮤텍스 동기화 시작
	mutex.lock();
	client_num--;

	//연결 해제된 해당 클라이언트의 정보를 삭제
	DeleteClientInfo(client_id);

	//closesocket()
	closesocket(client_sock);
	//끝
	mutex.unlock();
}

void TCPServer::PrintIPAndPort(const SOCKADDR_IN client_addr)
{
	std::cout << "[TCP 서버] 클라이언트 종료 IP 주소: " << inet_ntoa(client_addr.sin_addr) << ", " << "포트 번호: " << ntohs(client_addr.sin_port) << std::endl;
}

void TCPServer::Run()
{
	Accept();
}

void TCPServer::Close()
{
	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
}



