#include<stdio.h>
#include<winsock2.h>
#include<iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable : 4996)

int main()
{
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
	const char* message;
	char server_reply[2100];
	int recv_size;
	DWORD timeout = 5 * 1000;
	printf("\n\n****S E R V E R****\n\n");

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

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);


	if ((new_socket = accept(s, (struct sockaddr*)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");

		//recebe o login e usuario, verifica se ta certo com os dados regitrados e envia de volta uma resposta 

		while (true)
		{
			setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

			//Reply to the client
			/*message = "Hello Client , I have received your connection. But I have to go now, bye\n";
			send(new_socket, message, strlen(message), 0);*/
			//message = "conected";
			//send(new_socket, message, strlen(message), 0);

			//Receive a reply from the server
			if ((recv_size = recv(new_socket, server_reply, 2000, 0)) != SOCKET_ERROR)
			{
				server_reply[recv_size] = '\0';
				puts(server_reply);
				puts("recebido");
			}
			else
			{
				puts("recebeu nada");
			}

		}
	}
	

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
	}

	puts("FECHOU O SERVER");

	closesocket(s);
	WSACleanup();

	return 0;
}