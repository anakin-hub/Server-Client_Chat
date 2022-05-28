#include <string>
#include <iostream>

using namespace std;

string contatos[] {"Eduardo", "Alexia", "Mayara", "Enzo", "Proxima Pagina", "Sair"};
int id;
int action = 0;

void menu();
void chat(string id);

int main()
{
	string name = "anakin";
	string log;
	int senha = 123;
	int pass;

	cout << "Usuario: ";
	cin >> log;
	cout << "Senha: ";
	cin >> pass;

	cout << "Entrando... " << endl;
	
	if (log == name && pass == senha)
	{
		cout << string(50, '\n');

		while (action > -1)
		{
			cout << action << endl;
			switch (action)
			{
			case 1:
				chat(contatos[id]);
				break;
			case -2:
				cout << "você saiu!" << endl;
				break;
			default:
				menu();
				break;
			}

		}
	}
	return 0;
}

void menu()
{
	cout << "Pressione o numero do chat para entrar: " << endl;
	for (int i = 0; i < 6; i++)
	{
	
		if (i+1 < 5)
		{
			cout << "\t [" << i + 1 << "] - " << contatos[i];
			if (i % 2 == 1)
			{
				cout << " - online";
			}
			else
			{
				cout << " - offline";
			}
		}
		else
		{
			cout << "\n\n";
			cout << "\t [" << i+3 << "] - " << contatos[i];
		}
		cout << endl;
	}
	cout << "Digite o numero desejado: ";
	cin >> id;

	if (id < 7)
	{
		action = 1;
	}
	else if (id == 8)
	{
		cout << "carregando proxima pagina de contatos..." << endl;
		action = -1;
	}
	else
	{
		cout << "Fazendo Log off.." << endl;
		action = -1;
	}
}

void chat(string id)
{
	int n;
	cout << "Chat de " << id << ":\n";
	cout << endl;
	cout << id << ": olá\n";
	cout << "Voce: oii\n";
	cout << "o que deseja fazer?\n";
	cout << "[1] - enviar mensagem\n";
	cout << "[2] - apagar mensagem\n";
	cout << "[3] - editar mensagem\n";
	cout << "[5] - sair do chat\n";
	cout << "Pressione o numero: ";
	cin >> n;
}
