// ServerT.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Winsock2.h"
#include "error.h"
#include <ctime>

#pragma comment(lib, "WS2_32.lib")

SOCKADDR_IN SetSocParm(short sf, int port, u_long sa)
{
	SOCKADDR_IN soc;
	soc.sin_family = sf;				// тип сетевого адреса
	soc.sin_port = htons(port);			// номер порта
	soc.sin_addr.s_addr = sa;			// IP-адрес
	return soc;
}

int main()
{
	setlocale(NULL, "");

	SOCKET sS;		// сокет сервера
	SOCKET cS;		// сокет для обмена данными с клиентом 

	SOCKADDR_IN clnt;	// параметры сокета клиента
	SOCKADDR_IN serv;	// параметры сокета сервера

	WSADATA wsaData;

	char ibuf[50],		// буфер ввода 
		obuf[50];		// буфер вывода
	int libuf = 0,		// количество принятых байт
		lobuf = 0,		// количество отправленных байт
		lclnt = sizeof(clnt);		// размер SOCKADDR_IN

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());

		if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());

		serv = SetSocParm(AF_INET, 2000, INADDR_ANY);	// установка параметров сокета

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw  SetErrorMsgText("bind:", WSAGetLastError());

		if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
			throw  SetErrorMsgText("listen:", WSAGetLastError());

		while (true)
		{
			if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET)
				throw  SetErrorMsgText("accept:", WSAGetLastError());
			else
			{
				cout << "connected: " << inet_ntoa(clnt.sin_addr) << ":" << htons(clnt.sin_port) << endl << endl;

				clock_t start = clock();

				while (true)
				{
					if ((libuf = recv(cS, ibuf, sizeof(ibuf) + 1, NULL)) == SOCKET_ERROR)
						throw  SetErrorMsgText("recv:", WSAGetLastError());
					else if (libuf > 0)
					{
						cout << "message: " << ibuf << endl;
						strcpy(obuf, ibuf);
						if ((lobuf = send(cS, ibuf, sizeof(ibuf) + 1, NULL)) == SOCKET_ERROR)
							throw  SetErrorMsgText("send:", WSAGetLastError());
					}
					else break;
				}

				clock_t finish = clock();
				printf("\nmessaging time is %.2f seconds\n\n", ((double)(finish - start)) / ((double)CLOCKS_PER_SEC));
			}
		}

		if (closesocket(sS) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText << endl;
	}
	return 0;
}

