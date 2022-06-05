#include<stdio.h>
#include<winsock2.h>
#include<iostream>

#define MAX_CON 5
#define BF_SZ 100

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable : 4996)

struct chat_protocol
{
	int id;
	char nick[20];
	char msg[100];
};

struct _client
{
	bool con;			//Set true if a client is connected
	sockaddr_in addr;	//Client info like ip address
	SOCKET socket;		//Client socket
	fd_set fset;		//used to check if there is data in the socket
	int i;				//any piece of additional info
	chat_protocol cp;
};

void startserver();
int accept(_client* x);
int send(_client* x, char* buffer, int sz);
int send(_client* x, char* buffer, int sz);
void accept_clients();
void recv_client();
void Server_Status(int status);
void disconnect(_client* x);
void endserver();
void midcopy(char* input, char* output, int start_pos, int stop_pos);

//Variables
int cons = 0, c = 1;//numero maximo de conecções do servidor & conecções atuais
SOCKET s, new_socket;//s socket do server & new_socket socket do servidor
sockaddr_in server, client;
_client clients[MAX_CON];

int main()
{
	startserver();

	/*const char* message;
	char server_reply[500];
	int recv_size;
	c = sizeof(struct sockaddr_in);*/

	while (true)
	{

		accept_clients();
		
		recv_client();

	}

	puts("FECHOU O SERVER");

	endserver();

	return 0;
}

void startserver()
{
	WSADATA wsa;

	cout << "\n\n****S E R V E R****\n\n";

	cout << "\nInitialising Winsock...";

	//Inicialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code :" << WSAGetLastError();
	}
	else
	{
		cout << " Done!\n";
	}

	//Create a Socket
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		cout << "SOCKET ERROR : Invalid socket." << endl;
	}
	else if (s == SOCKET_ERROR)
	{
		cout << "SOCKET ERROR : Socket error." << endl;
	}
	else
	{
		cout << "SOCKET ESTABLISHED" << endl;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&c, sizeof(c));

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError();
	}

	cout << "Bind done";

	//Listen to incoming connections
	listen(s, 5);

	unsigned long b = 1;
	ioctlsocket(s, FIONBIO, &b);

	//Accept and incoming connection
	cout << "\nWaiting for incoming connections...\n";
	for (int x = 0; x < MAX_CON; x++) { clients[x].con = FALSE; }
}

int accept(_client* x)
{
	x->i = sizeof(sockaddr);
	x->socket = accept(s, (sockaddr*)&x->addr, &x->i);

	if (x->socket != 0 && x->socket != SOCKET_ERROR)
	{
		x->con = true;
		FD_ZERO(&x->fset);
		FD_SET(x->socket, &x->fset);
		return (true);
	}
	return (false);
}

int send(_client* x, char* buffer, int sz)
{
	x->i = send(x->socket, buffer, sz, 0);
	if (x->i == SOCKET_ERROR || x->i == 0)
	{
		cout << "nao enviou"<<sz<<"\t"<<x->i<< "\t" <<WSAGetLastError() << endl;
		disconnect(x);
		return (false);
	}
	else
	return (true);
}

int recv(_client* x, char* buffer, int sz)
{
	

	if (FD_ISSET(x->socket, &x->fset))//problema
	{
		//cout << "info a receber\n";
		x->i = recv(x->socket, buffer, sz, 0);
		if (x->i == 0)
		{
			cout << "nao recebeu\n";
			disconnect(x);
			return (false);
		}
		//FD_ZERO(&x->fset);
		return (true);
	}

	return (false);
}

void accept_clients()
{
	if (cons < MAX_CON)
	{
		for (int i = 0; i < MAX_CON; i++)
		{
			if (!clients[i].con)
			{
				if (accept(&clients[i]))
				{
					//update server status
					Server_Status(1);
				}
			}
		}
	}
}

void Server_Status(int status)
{
	if (status == 1)
	{
		cons++;
		cout << "\n a client has connected";
	}
	else if (status == -1)
	{
		cons--;
		cout << "\n a client has disconnected";
	}
	else
	{
		//never leave out anything
		cout << "\n>>>>>>We got an unknown message :" << status;
	}
}

void disconnect(_client* x) //this is called by the low level funtions
{
	char m[] = "/quited\0";
	send(x,m, BF_SZ);
	if (x->socket)closesocket(x->socket);
	x->con = false;
	x->i = -1;
	Server_Status(-1);
}

void endserver()
{ 
	for (int i = 0; i < MAX_CON; i++) { disconnect(&clients[i]); }
	cout << "ending server\n";
	closesocket(s);
	WSACleanup();
}

void chat_message(char* s)
{
	for (int i = 0; i < MAX_CON; i++)
	{
		if (clients[i].con)	
		{
			send(&clients[i], s, BF_SZ);
		}
	}
}

void recv_client()
{
	char buffer[BF_SZ];


		for (int i = 0; i < MAX_CON; i++)
		{
			if (clients[i].con)
			{
				memset(buffer, 0, BF_SZ);
				if (recv(&clients[i], buffer, BF_SZ))
				{
					if (buffer[0] == '/')
					{
						//respond to commands
						if (strcmp(buffer, "/server_bang") == 0)
						{
							char c[] = "8*8* The Server Goes BANG *8*8";
							chat_message(c);
						}
						else if (strcmp(buffer, "/quit") == 0)
						{
							disconnect(&clients[i]);
							char c[] = "\na client has gone\n";
							chat_message(c);
							cout << c << endl;
						}
						else if (strstr(buffer, "/log") != NULL)
						{
							midcopy(buffer, clients[i].cp.nick, 4, sizeof(clients[i].cp.nick));
							cout << clients[i].cp.nick << endl;
							char c[30];
							sprintf(c, "%s está conectado", clients[i].cp.nick);
							chat_message(c);
						}
						else if (strcmp(buffer, "/users") == 0)
						{
							memset(buffer, 0, BF_SZ);
							for (int j = 0; j < MAX_CON; j++)
							{
								if (clients[j].con)
								{
									strcat(buffer, clients[j].cp.nick);
									strcat(buffer, "\n");
								}
							}
							send(&clients[i], buffer, BF_SZ);
						}
						else if (strstr(buffer, "/tell") != NULL)
						{
							char c[120];
							midcopy(buffer, c, 5, sizeof(buffer));
							memset(buffer, 0, BF_SZ);
							for (int n = 0; n < MAX_CON; n++)
							{
								int nlen = strlen(clients[n].cp.nick);
								if (nlen > 0)
								{
									char nm[20];
									midcopy(c, nm, 0, nlen);
									if (strcmp(nm, clients[n].cp.nick) == 0)
									{	
										midcopy(c, c, nlen, sizeof(c));
										sprintf(buffer, "%s enviou uma mensagem:%s", clients[i].cp.nick, c);
										send(&clients[n], buffer, BF_SZ);
									}
								}
							
							}
						}
						else
						{
							cout << endl;
							cout << buffer << endl;
							//cout << "recebeu algo com / \n";

							chat_message(buffer);
						}
					}
					else
					{
						/*cout << "tenta enviar mensagem\n";
						chat_message(buffer);*/
					}
				}
			}
		}
}

void midcopy(char* input, char* output, int start_pos, int stop_pos)
{
	int index = 0;

	for (int i = start_pos; i < stop_pos; i++)
	{
		output[index] = input[i];
		index++;
	}

	output[index] = 0;
}

//void receive_data()
//{
//	char buffer[BUFFER_SIZE];
//
//	for (int j = 0; j < MAX_CLIENTS; j++)
//	{
//		if (client[j].connected)
//		{
//			//cout << "usuario: " << j << "conected!\n"; //erro no codigo, não detecta quem desconecta no instante
//			if (receive_client(&client[j], buffer, BUFFER_SIZE))
//			{
//				if (buffer[0] == '~')
//				{ // All data should be buffered by a '~' just because
//
//					if (buffer[1] == '1') // Add Client Command
//					{
//						// Declare the buffer to store new client information into
//						char raw_data[BUFFER_SIZE];
//
//						// Parse out the 'Add Client' command
//						midcopy(buffer, raw_data, 3, strlen(buffer));
//
//						// Store the client information into our RAM client database
//						sscanf(raw_data, "%s %s %s", client[j].template_name, client[j].screen_name, client[j].siegepos);
//
//						for (int k = 0; k < MAX_CLIENTS; k++)
//						{
//							if ((client[k].connected) && (j != k))
//							{
//								// Parse in the client data to send
//								sprintf(raw_data, "~1 %s %s %s", client[k].template_name, client[k].screen_name, client[k].siegepos);
//
//								// Send the client data
//								send_data(&client[j], raw_data, BUFFER_SIZE);
//							}
//						}
//					}
//					else if (buffer[1] == '2') // Move Client Command
//					{
//						// Declare the buffer to store new client information into
//						char raw_data[BUFFER_SIZE];
//
//						// Parse out the 'Move Client' command
//						midcopy(buffer, raw_data, 3, strlen(buffer));
//
//						// Update the client information into our RAM client database
//						sscanf(raw_data, "%s %s", client[j].screen_name, client[j].siegepos);
//					}
//					else if (buffer[1] == '3') // Chat Client Command
//					{
//						// ECHO THE MESSAGE BACK TO ALL CLIENTS
//					}
//					else if (buffer[1] == '4') // Remove Client Command
//					{
//						// Disconnect the current client
//						disconnect_client(&client[j]);
//					}
//
//					// Display all data received
//					// cout << buffer << endl;
//
//					// Echo the message to the other clients
//					echo_message(buffer);
//
//					// Clear the buffer
//					buffer[0] = '/0';
//				}
//			}
//		}
//	}