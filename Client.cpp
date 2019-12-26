/* Client */
#include <cstdio>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define THROW_WSA printf("\n\nLast error code: %d\n\n", WSAGetLastError())
#define SOCK_ASRT(expr)\
	if ((expr) == SOCKET_ERROR)\
		THROW_WSA;

/* server configs */
#define SC_AF           AF_INET
#define SC_TYPE         SOCK_STREAM
#define SC_PROTO        0
#define SC_PORT         80

/* client configs */
#define CC_LOCALHOST_IP "127.0.0.1"
#define CC_BUFFER_SIZE  1024

/* server messages */
#define SM_EXIT         "exit"

SOCKET connectLocal() {
	WSADATA data;

	SOCK_ASRT(
		WSAStartup(MAKEWORD(2, 2), &data)
	);

	SOCKET clientSock;
	sockaddr_in addr;

	SOCK_ASRT(
		clientSock = socket(
			SC_AF,
			SC_TYPE,
			SC_PROTO
		)
	);

	addr.sin_family = SC_AF;
	addr.sin_port = htons(SC_PORT);
	addr.sin_addr.S_un.S_addr = inet_addr(CC_LOCALHOST_IP);

	SOCK_ASRT(
		connect(
			clientSock,
			(sockaddr*)&addr,
			sizeof(sockaddr)
		)
	);

	return clientSock;
}

void recvData(SOCKET* sock, char* buffer, size_t len) {
	ZeroMemory(buffer, len);

	SOCK_ASRT(
		recv(
			*sock,
			buffer,
			len,
			0
		)
	);
}

void sendData(SOCKET* sock, char* data) {
	SOCK_ASRT(
		send(
			*sock,
			data,
			strlen(data),
			0
		)
	);
}

int main() {
	printf("Connect to host... ");

	SOCKET clientSock = connectLocal();

	printf("OK\n");

	char* recvBuffer;
	char* sendBuffer;

	while (true) {
		recvBuffer = new char[CC_BUFFER_SIZE];
		sendBuffer = new char[CC_BUFFER_SIZE];

		printf("Input resource name: ");
		scanf_s("%s", sendBuffer, CC_BUFFER_SIZE);

		sendData(&clientSock, sendBuffer);

		if (!strcmp(sendBuffer, SM_EXIT)) {
			printf("Disconnect...\n");
			break;
		}

		recvData(&clientSock, recvBuffer, CC_BUFFER_SIZE);

		printf("\n\nRecv:\n%s\n\n", recvBuffer);
		
		delete[] recvBuffer;
		delete[] sendBuffer;
	}


	closesocket(clientSock);
	WSACleanup();

	printf("Exit...\n\n");

	return 0;
}