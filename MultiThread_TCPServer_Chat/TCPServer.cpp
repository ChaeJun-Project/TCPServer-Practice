#include "stdafx.h"
#include "TCPServer.h"

TCPServer::TCPServer()
{
	//Initialize
	assert(Initialize());
	std::cout << "[B477040 ä�� TCP_Chat_Server]" << std::endl;
}

TCPServer::~TCPServer()
{
    //client_infos���Ϳ� �ִ� ��� ������ �޸� ����
	for (int i = 0; i < client_infos.size(); ++i)
	{
		delete client_infos[i];
	}
	Close();
}

const bool TCPServer::Initialize()
{
	//���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	//���� �ʱ�ȭ
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		ErrorCheck::Get().err_quit("socket()");
		return false;
	}

	//socket_addr �ʱ�ȭ
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

		//buf�� ������ �� ����
		ZeroMemory(buf, sizeof(buf));

		//Create Client ID
		CreateClientID();

		//���ۿ� Client ID�� ����
		strcpy(buf, Convert::ConvertStringToChar(client_id));

		//Client ID �ʱ�ȭ
		client_id = "Client";

		//client id�� ��Ʈ��ȣ�� �����ϱ� ����
		strcat(buf, ",");
		//���ۿ� Client Port��ȣ�� ����
		strcat(buf, Convert::ConvertStringToChar(std::to_string(client_addr.sin_port)));

		//����� Ŭ���̾�Ʈ���� Client ID�� ����
		retval = send(client_sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			return;
		}

		//PacketConnect ��Ŷ�� ����
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			return;
		}
		else if (retval == 0)
			return;

		//PacketConnect ��Ŷ�� �����͸� ���
		std::cout << buf << std::endl;

		//buf�� ���� �� ���ῡ ������ Client ID�� ����
		std::string client_id = Utility::GetClientIDFromData(buf);

		//���� ����� Ŭ���̾�Ʈ�� ������ session�� �ִ� client_infos ���Ϳ� �߰�
		AddClientInfo(new ClientInfo(client_sock, client_addr, client_id));

		//Send ClientAcceptPacket
		SendAccept(client_id);

		//������ Ǯ�� this�����Ϳ� ������ ���� client_id�� ĸó�ϰ� CommunicateData()�Լ��� Ǯ�� ���
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
	//buf�� ������ �� ����
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

	//�ٿ� ĳ����
	PacketAccept *packet_accept = dynamic_cast<PacketAccept*>(packet);

	//ServerLog.txt ���Ͽ� ���� ������ ID�� ���
	WriteFile(packet_accept->GetLogStr());

	delete packet;
}

void TCPServer::SendDisAccept(const std::string & client_id)
{
	//buf�� ������ �� ����
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

	//�ٿ� ĳ����
	PacketDisAccept *packet_disaccept = dynamic_cast<PacketDisAccept*>(packet);

	//ServerLog.txt ���Ͽ� ���� ������ ID�� ���
	WriteFile(packet_disaccept->GetLogStr());

	

	delete packet;
}

void TCPServer::WriteFile(const std::string& log)
{
    //log���� ���
	std::cout << log << std::endl;

	std::ofstream write_file;
	//std::fstream::app�� ������ ���Ͽ� ������ �� �߰��ؼ� �ۼ��ϴ� �ɼ���
	write_file.open(server_log, std::fstream::app);

	if (write_file.is_open())
	{
		write_file << log;

		//�ٹٲ�
		write_file << std::endl;
	}

	write_file.close();
}

void TCPServer::SendData(const char * buf)
{
	char buffer[BUFSIZE];

	//��Ŷ ������ �����Ϳ� ���Ե� ��� �����Ϳ��� client_id�� ���� 
	std::string client_id = Utility::GetClientIDFromDataWithPacketType(buf);

	//Client_ID�� ���ԵǾ� �ִ� �����Ϳ��� ���� �ٸ� Ŭ��� ���� �����͸� ����
	std::string message = Utility::GetMessageFromData(buf);

	//PacketServerSend��Ŷ �����Ҵ�
	PacketHeader *packet = new PacketServerSend(client_id, std::to_string(GetClientInfo(client_id)->client_addr.sin_port), message);

	//�����͸� ���� ���ۿ� ������ ��Ŷ�� �����͸� �ѱ�
	strcpy(buffer, Convert::ConvertStringToChar(packet->GetDataMessage()));

	//client_infos�� ����� ������ ��� ��ȸ�ϸ鼭
	//�ش� client_id�� ������ ���� id�� �ٸ� ��츸 �����͸� ����
	for (int i = 0; i < client_infos.size(); ++i)
	{
		if (client_infos[i]->client_id != client_id)
		{
			retval = send(client_infos[i]->client_sock, buffer, sizeof(buffer), 0);
			assert(retval);
		}
	}

	//�����Ҵ��� ��Ŷ ����
	delete packet;
}

void TCPServer::CommunicateData(const std::string& client_id)
{
	// Ŭ���̾�Ʈ ���� ���
	SOCKET client_sock = GetClientInfo(client_id)->client_sock;
	SOCKADDR_IN client_addr = GetClientInfo(client_id)->client_addr;
	
	//buf�� ������ �� ����
	ZeroMemory(buf, sizeof(buf));

	// Ŭ���̾�Ʈ�� ������ ���
	while (1)
	{
		//buf�� ������ �� ����
		ZeroMemory(buf, sizeof(buf));

		//������ �ޱ�
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			break;
		}
		else if (retval == 0)
			break;

	    //Ŭ���̾�Ʈ�� ���������� ������ �������� ���
		if (Utility::GetPacketTypeFromData(buf) == static_cast<int>(PT_CLIENT_DISCONNECT))
		{
			//������ ������ Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ��ȣ�� ���
			PrintIPAndPort(client_addr);
		    std::cout<< Utility::GetWithoutPacketTypeFromData(buf) <<std::endl;
		    break;
		}

		//Ŭ���̾�Ʈ�� ���������� �޽����� ������ ���
		if (Utility::GetPacketTypeFromData(buf) == static_cast<int>(PT_CLIENT_SEND))
		{
			//���� Ŭ���̾�Ʈ�� ������ ������ Ŭ��鿡�� �޽��� ����
			SendData(buf);
		}

	}

	//������ ���� ����
	if (retval == SOCKET_ERROR)
	{
		//������ ������ Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ��ȣ�� ���
		PrintIPAndPort(client_addr);
		std::cout << client_id <<", disconnection request success" << std::endl;
	}

	//PacketDisAccept ��Ŷ�� ���� ���� ���� Ŭ���̾�Ʈ�� ����
	SendDisAccept(client_id);

	//���ؽ� ����ȭ ����
	mutex.lock();
	client_num--;

	//���� ������ �ش� Ŭ���̾�Ʈ�� ������ ����
	DeleteClientInfo(client_id);

	//closesocket()
	closesocket(client_sock);
	//��
	mutex.unlock();
}

void TCPServer::PrintIPAndPort(const SOCKADDR_IN client_addr)
{
	std::cout << "[TCP ����] Ŭ���̾�Ʈ ���� IP �ּ�: " << inet_ntoa(client_addr.sin_addr) << ", " << "��Ʈ ��ȣ: " << ntohs(client_addr.sin_port) << std::endl;
}

void TCPServer::Run()
{
	Accept();
}

void TCPServer::Close()
{
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
}



