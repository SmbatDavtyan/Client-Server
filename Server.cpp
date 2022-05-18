#include <iostream>
#include <thread>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

// Winsock library

#define RW_BUFFSIZE 1024
#define LOG_TEXT(log_text) std::cout << log_text << std::endl

void send_data(SOCKET Socket)
{
	char input[RW_BUFFSIZE]{};

	while (strcmp(input, "_quit_") != 0)
	{
		if (gets_s(input, RW_BUFFSIZE))
		{
			send(Socket, input, RW_BUFFSIZE, 0);
		}
	}
}

void receive_data(SOCKET Socket)
{
	char output[RW_BUFFSIZE]{};

	while (strcmp(output, "_quit_") != 0)
	{
		int is_resv = recv(Socket, output, RW_BUFFSIZE, 0);

		if (is_resv > 0)
		{
			std::cout << "		" << output << std::endl;
		}
	}
}

int main()
{
	// Initialize winsock
	WSAData winsock_data;
	WORD wrd = MAKEWORD(2, 2);
	int winsock_res = WSAStartup(wrd, &winsock_data);

	if (winsock_res != 0)
	{
		LOG_TEXT("Winsock failed to initialize.");
		return -1;
	}

	// Create listen socket
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		LOG_TEXT("failed to create socket.");
		return -1;
	}
	
	// Create socket info
	sockaddr_in sock_info;
	sock_info.sin_family = AF_INET;
	sock_info.sin_port = htons(54000);
	sock_info.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listen_socket, (sockaddr*)&sock_info, sizeof(sock_info));

	// make socket listen socket
	listen(listen_socket, SOMAXCONN);

	// wait for connection
	sockaddr_in client;
	int client_size = sizeof(client);

	SOCKET client_socket = accept(listen_socket, (sockaddr*)&client, &client_size);
	
	if (client_socket == INVALID_SOCKET)
	{
		LOG_TEXT("failed to create socket.");
		return -1;
	}

	closesocket(listen_socket);

	std::thread read_thread(receive_data, client_socket);
	std::thread write_thread(send_data, client_socket);

	read_thread.join();
	write_thread.join();

	closesocket(client_socket);
	WSACleanup();

	return 0;
}
