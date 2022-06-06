#include<stdio.h>
#include<winsock2.h>
#include<iostream>
#include<fstream>
#include"Message.h"

#define MAX_CON 5
#define BF_SZ 100
#define MAX_MSG 10

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable : 4996)

struct _client
{
	bool con;			//Set true if a client is connected
	sockaddr_in addr;	//Client info like ip address
	SOCKET socket;		//Client socket
	fd_set fset;		//used to check if there is data in the socket
	int i;				//any piece of additional info
	char nick[20];		//Client's nickname to connect
	int id;				//number to identify the Client into program
};

void startserver();//ok
int accept(_client* x);//ok
int send(_client* x, char* buffer, int sz);//ok
int recv(_client* x, char* buffer, int sz);//ok 
void accept_clients();//ok
void recv_client();
void Server_Status(int status, char nick[20]);//ok
void disconnect(_client* x);//ok
void endserver();//ok
void midcopy(char* input, char* output, int start_pos, int stop_pos);//ok
void loadProfiles();//PRECISA CRIAR O ARQUIVO
void addMessage(char* buffer, int sz, int id);//ok
void loadMessage();//ok
void saveMessage();//ok
void delMessage(_client* x, int msg, int id);//PRECISA ARRUMAR NO CODIGO E AJEITAR A FUNÇÃO
void editMessage(_client* x, int id, int msg, char* buffer);//PRECISA ARRUMAR NO CODIGO E AJEITAR A FUNÇÃO
void removeMessage(int msg);//OK


//Variables
int cons = 0, c = 1;//numero maximo de conecções do servidor & conecções atuais
SOCKET s, new_socket;//s socket do server & new_socket socket do servidor
sockaddr_in server, client;
_client clients[MAX_CON];
int chatm = 0; //numero de mensagens salvas pelo servidor
Message history[MAX_MSG];

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
	loadProfiles();
	loadMessage();
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
		disconnect(x);
		return (false);
	}
	else
	return (true);
}

int recv(_client* x, char* buffer, int sz)
{
	if (FD_ISSET(x->socket, &x->fset))
	{
		x->i = recv(x->socket, buffer, sz, 0);
		if (x->i == 0)
		{
			disconnect(x);
			return (false);
		}
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
					Server_Status(1, clients[i].nick);
				}
			}
		}
	}
}

void Server_Status(int status, char nick[20])
{
	char c[100];
	if (status == 1)
	{
		cons++;
		sprintf(c, "\n %s has connected", nick);
		cout << c;
	}
	else if (status == -1)
	{
		cons--;
		sprintf(c, "\n %s has disconnected", nick);
		cout << c;
	}
	else
	{
		//never leave out anything
		cout << "\nWe got an unknown message :" << status;
	}
}

void disconnect(_client* x) //this is called by the low level funtions
{
	char m[] = "/quited\0";
	send(x,m, BF_SZ);
	if (x->socket)closesocket(x->socket);
	x->con = false;
	x->i = -1;
	Server_Status(-1, x->nick);
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
	int idv;

		for (int i = 0; i < MAX_CON; i++)
		{
			if (clients[i].con)
			{
				memset(buffer, 0, BF_SZ);

				if (recv(&clients[i], buffer, BF_SZ))
				{	
					//cout << "\tb0:" << buffer[0] << "\tb1:" << buffer[1] << endl;
					//system("pause");
					if (buffer[0] >= 0+48 && buffer[0] <= 9+48/* && idv == clients[i].id*/)
					{
						idv = buffer[0];
						idv -= 48;
						cout << "cliente reconhecido" << endl;
						midcopy(buffer, buffer, 1, BF_SZ);
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
							else if (strcmp(buffer, "/users") == 0)
							{
								memset(buffer, 0, BF_SZ);
								for (int j = 0; j < MAX_CON; j++)
								{
									if (clients[j].con)
									{
										strcat(buffer, clients[j].nick);
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
									int nlen = strlen(clients[n].nick);
									if (nlen > 0)
									{
										char nm[20];
										midcopy(c, nm, 0, nlen);
										if (strcmp(nm, clients[n].nick) == 0)
										{
											midcopy(c, c, nlen, sizeof(c));
											sprintf(buffer, "%s enviou uma mensagem:%s", clients[i].nick, c);
											send(&clients[n], buffer, BF_SZ);
										}
									}

								}
							}
							else if (strstr(buffer, "/hist") != NULL)
							{
								memset(buffer, 0, BF_SZ);
								char c[2050];
								for (int b = 0; b < chatm; b++)
								{
									for (int a = 0; a < MAX_CON; a++)
									{
										if (a == 0 && b == 0)
										{
											sprintf(c, "___HISTÓRICO___\n");
										}
										if (history[b].getId() == clients[a].id)
										{
											memset(buffer, 0, BF_SZ);
											sprintf(buffer, "[%d] %s: %s\n", b, clients[a].nick, history[b].getText());
											strcat(c, buffer);
										}
										if (b == chatm-1 && a == MAX_CON-1)
										{
											strcat(c, "___FIM___\n");
										}
									}
								}
								cout << c << endl;
								send(&clients[i], c, 2050);
							}
							else if (strstr(buffer, "/edit") != NULL)
							{
								int msg = buffer[5] - 48;
								midcopy(buffer, buffer, 6, sizeof(buffer));
								editMessage(&clients[i], idv, msg, buffer);
							}
							else if (strstr(buffer, "/del") != NULL)
							{
								int msg = buffer[4] - 48;

								delMessage(&clients[i], msg, idv);
							}
							
						}
						else
						{
							cout << endl;
							cout << buffer << endl;
							addMessage(buffer, BF_SZ, idv);
							char* msg = new char[BF_SZ];
							sprintf(msg, "%s: %s\n", clients[i].nick, buffer);
							chat_message(msg);
							saveMessage();
						}
					}
					else if (strstr(buffer, "/log") != NULL)
						{
						midcopy(buffer, clients[i].nick, 4, sizeof(clients[i].nick));
						cout << clients[i].nick << endl;
						char c[30];
						sprintf(c, "%d", clients[i].id);
						send(&clients[i], c, 20);

						/*memset(c, 0, 20);
						sprintf(c, "%s está conectado", clients[i].nick);
						chat_message(c);*/						
						}
					//else if (strstr(buffer, "/log") != NULL)
					//{
					//	char c[20];
					//	sprintf(c, "/log%s", clients[i].nick);
					//	cout << "\npassou do log\n" << c << endl << buffer << endl;
					//	if (strcmp(buffer, c) == 0)
					//	{
					//		cout << "\naqui\n";
					//		memset(c, 0, 20);
					//		sprintf(c, "%d", clients[i].id);
					//		send(&clients[i], c, 20);
					/*		memset(c, 0, 20);
							sprintf(c, "%s esta conectado", clients[i].nick);
							chat_message(c);*/
					//	}
					//}
					
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

void loadProfiles()
{
	ifstream ifs("profiles.txt", ios::in);

	if (ifs.is_open())
	{
		int idt;
		for (int i = 0; i < MAX_CON; i++)
		{
			ifs >> idt;
			clients[i].id = idt;
		}
		ifs.close();
	}
}

void addMessage(char* buffer, int sz, int id)
{
	if (chatm < MAX_MSG)
	{
		history[chatm].setId(id);
		history[chatm].setText(buffer);
		chatm++;
	}
	else
	{
		removeMessage(1);
		addMessage(buffer, sz, id);
	}
}

void loadMessage()
{
	ifstream ifs("historico.txt", ios::in);

	if (ifs.is_open())
	{
		ifs >> chatm;
		int idt;
		char textot[100];
		for (int i = 0; i < chatm; i++)
		{
			ifs >> idt;
			ifs.ignore();
			ifs.getline(textot, 100);
			history[i].setId(idt);
			history[i].setText(textot);
		}
		ifs.close();
	}
}

void saveMessage()
{
	ofstream ifs;
	ifs.open("historico.txt");
	if (ifs.is_open())
	{
		ifs << chatm << endl;
		for (int i = 0; i < chatm; i++)
		{
			ifs << history[i].getId() << endl;
			ifs << history[i].getText() << endl;
		}

		ifs.close();
	}
}

void delMessage(_client* x, int msg, int id)
{
	if (history[msg].getId() == id)
	{
		removeMessage(msg);
		char c[] = "mensagem excluida com sucesso\n";
		send(x, c, sizeof(c));
	}
	else
	{
		char c[] = "essa mensagem nao pode ser excluida\n";
		send(x, c, sizeof(c));
	}
}

void editMessage(_client *x, int id, int msg, char* buffer)
{
	if (history[msg].getId() == id)
	{
		history[msg].setText(buffer);
		saveMessage();
		char c[] = "mensagem salva com sucesso\n";
		send(x, c, sizeof(c));
	}
	else
	{
		char c[] = "essa mensagem nao pode ser editada\n";
		send(x, c, sizeof(c));
	}
}

void removeMessage(int msg)
{
	for (int i = msg; i < chatm; i++)
	{
		history[i - 1] = history[i];
	}
	chatm--;
	saveMessage();
}
