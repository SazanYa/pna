// ClientT.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Winsock2.h"
#include "error.h"

#pragma comment(lib, "WS2_32.lib")

SOCKADDR_IN SetSocParm(short sf, int port, u_long sa)
{
	SOCKADDR_IN soc;
	soc.sin_family = sf;				// тип сетевого адреса
	soc.sin_port = htons(port);			// номер порта
	soc.sin_addr.s_addr = sa;			// IP-адрес
	return soc;
}

bool IsNumber(char *str)
{
	bool isnum = true;
	while (*str)
		if (!isdigit(*str++))
		{
			isnum = false;
			break;
		}
	return isnum;
}

int GetInt()
{
	char buf[10];
	do cin >> buf; while (!IsNumber(buf));
	return atoi(buf);
}

int main()
{
	setlocale(NULL, "");

	SOCKET  sS;
	WSADATA wsaData;

	char ibuf[50],		// буфер ввода 
		obuf[50] = "Hello from Client ",	// буфер вывода
		inbuf[50],		// вспомогательный буфер
		nbuf[10];		// буфер для номера сообщения
	int libuf = 0,		// количество принятых байт
		lobuf = 0,		// количество отправленных байт
		msg_count;		// количество сообщений

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("startup: ", WSAGetLastError());

		if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket: ", WSAGetLastError());

		SOCKADDR_IN serv = SetSocParm(AF_INET, 2000, inet_addr("127.0.0.1"));	// установка параметров сокета

		if ((connect(sS, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
			throw  SetErrorMsgText("connect: ", WSAGetLastError());

		cout << "Enter a count of messages: "; 
		msg_count = GetInt();

		for (int i = 1; i <= msg_count; i++)
		{
			_itoa(i, nbuf, 10);

			strcpy(inbuf, obuf);	// копируем сообщение
			strcat(inbuf, nbuf);	// добавляем к сообщению его номер

			if ((lobuf = send(sS, inbuf, sizeof(inbuf) + 1, NULL)) == SOCKET_ERROR)
				throw  SetErrorMsgText("send:", WSAGetLastError());

			if ((libuf = recv(sS, ibuf, sizeof(ibuf) + 1, NULL)) == SOCKET_ERROR)
				throw  SetErrorMsgText("recv:", WSAGetLastError());
			else
				cout << "message: " << ibuf << endl;
		}

		if (closesocket(sS) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText << endl;
	}
    return 0;
}

