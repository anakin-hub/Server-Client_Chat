#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <thread>

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable : 4996)

struct chat_protocol
{
	int id;
	char nick[20];
	char msg[100];
};

//Function's call
void send_data();
void recv_data();
void disconect(int signal);
void login();

//Variables
SOCKET s;
chat_protocol protc;

int main()
{
	WSADATA wsa;
	struct sockaddr_in server;
	
	
	
	
	cout << "Insira o nick: ";
	cin >> protc.nick;

	cout <<"\t" << protc.nick << endl;
	cout << "SEJA BEM VINDO\n";
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	server.sin_addr.s_addr = inet_addr("192.168.100.27");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	puts("Connected");

	login();
	cin.ignore();
	
	thread receiveData(recv_data);
	//receiveData.join();
	thread sendData(send_data);
	//sendData.join();
	
	while (true)
	{
		

		
	}

	printf("acabou");

	closesocket(s);
	WSACleanup();

	return 0;
}

void send_data()
{
	char* message = new char[50];
	int check;
	while (true)
	{
		memset(message, 0, sizeof(message));
		cout << "Digite uma mensagem: ";
		cin.getline(message, 50);
		cout << message << endl;

		check = send(s, message, strlen(message), 0);

		if (check == 0)
		{
			puts("\nSERVER terminated connection");
			disconect(check);
		}
		else if (check == SOCKET_ERROR)
		{
			puts("\nSOCKET error");
			disconect(check);
		}
		puts("Data Send\n");
	}
}

void recv_data()
{
	char server_reply[2100];
	int recv_size, check;
	while (true)
	{
		check = recv(s, server_reply, 2000, 0);
		recv_size = check;

		//Receive a reply from the server
		if (check == SOCKET_ERROR)
		{
			puts("receive failed");
		}
		else if (check > 0)
		{
			server_reply[recv_size] = '\0';
			cout << "\nReceived message: ";
			puts(server_reply);
			if (strcmp(server_reply, "/quited") == 0)
			{
				disconect(check);
			}
		}
	}
}

void disconect(int signal)
{
	if (s)
		closesocket(s);
	WSACleanup();
	cout << "EXIT SIGNAL :" << signal;
	exit(0);
}

void login()
{
	char msg[20];
	sprintf(msg, "/log %s", protc.nick);
	send(s, msg, 20, 0);
}
