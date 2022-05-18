#include <string>
#include <iostream>
#include <thread>
#include <WS2tcpip.h>

// Winsock library

#define RW_BUFFSIZE 1024
#define LOG_TEXT(log_text) std::cout << log_text << std::endl

#pragma comment(lib, "ws2_32.lib")

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

		if (is_resv > 0) {
			std::cout << "		" << output << std::endl;
		}
	}
}

int main()
{
	// Server local IP address and listening port
	const int port = 54000;
	const char* server_ip = "127.0.0.1";

	// Initialize winsock
	WSAData winsock_data;
	WORD wrd = MAKEWORD(2, 2);
	int winsock_res = WSAStartup(wrd, &winsock_data);

	if (winsock_res != 0)
	{
		LOG_TEXT("Winsock failed to initialize.");
		return -1;
	}

	// Create socket information
	sockaddr_in sock_info;
	sock_info.sin_family = AF_INET;
	sock_info.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &sock_info.sin_addr);

	// Create socket
	SOCKET socket_inst = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_inst == INVALID_SOCKET)
	{
		LOG_TEXT("failet to create socket");
		return -1;
	}

	// connect to server server_ip

	int is_connected = connect(socket_inst, (sockaddr*)&sock_info, sizeof(sock_info));

	if (is_connected == SOCKET_ERROR)
	{
		LOG_TEXT("Connection to server failed.");
		return -1;
	}
	
	std::thread read_thread(receive_data, socket_inst);
	std::thread write_thread(send_data, socket_inst);

	read_thread.join();
	write_thread.join();

	closesocket(socket_inst);
	WSACleanup();

}
