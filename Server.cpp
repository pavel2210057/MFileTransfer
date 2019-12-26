/* Server */
#include <cstdio>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define THROW_WSA printf("Last error code: %d\n", WSAGetLastError())
#define SOCK_ASRT(expr)\
	if ((expr) == SOCKET_ERROR)\
		THROW_WSA;

/* server configs */
#define SC_AF           AF_INET
#define SC_TYPE         SOCK_STREAM
#define SC_PROTO        0
#define SC_PORT         80
#define SC_BUFFER_SIZE  1024

/* server messages */
#define SM_EXIT         "exit"

template <typename T, typename U>
struct Pair {
	T first;
	U second;

	Pair(T first, U second) {
		this->first = first;
		this->second = second;
	}
};

SOCKET startServer() {
	WSADATA data;

	SOCK_ASRT(
		WSAStartup(MAKEWORD(2, 2), &data)
	);

	SOCKET servSock;
	sockaddr_in addr;

	SOCK_ASRT(
		servSock = socket(
			SC_AF,
			SC_TYPE,
			SC_PROTO
		)
	);

	addr.sin_family = SC_AF;
	addr.sin_port = htons(SC_PORT);
	addr.sin_addr.S_un.S_addr = ADDR_ANY;

	SOCK_ASRT(
		bind(
			servSock,
			(sockaddr*)&addr,
			sizeof(sockaddr)
		)
	);

	return servSock;
}

Pair<SOCKET, sockaddr> acceptClient(SOCKET* sock) {
	SOCKET clientSock = {};
	sockaddr addr = {};
	int len = sizeof(sockaddr);

	SOCK_ASRT(listen(*sock, 0));
	SOCK_ASRT(clientSock = accept(*sock, &addr, &len));
	
	return Pair<SOCKET, sockaddr>(clientSock, addr);
}

void getFileContentPart(FILE* f, char** buffer, size_t len) {
	fread_s(
		*buffer,
		len,
		sizeof(char),
		len,
		f
	);
}

size_t getFileContent(char* file, char** buffer, size_t len) {
	ZeroMemory(*buffer, len);

	FILE* f;
	fopen_s(&f, file, "r");

	if (!f)
		return -1;

	getFileContentPart(f, buffer, SC_BUFFER_SIZE);
	
	fclose(f);

	return strlen(*buffer);
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
	printf("Start server... ");
	
	SOCKET serverSock = startServer();

	printf("OK\nWaiting client... ");

	auto clientSock = acceptClient(&serverSock);
	
	printf("Connected\n");

	char* recvBuffer;
	char* fileContentBuffer;
	char* sendBuffer;
	size_t len;
	
	while (true) {
		recvBuffer = new char[SC_BUFFER_SIZE];
		fileContentBuffer = new char[SC_BUFFER_SIZE];
		sendBuffer = new char[SC_BUFFER_SIZE];
		
		printf("Waiting client request...\n");

		recvData(&clientSock.first, recvBuffer, SC_BUFFER_SIZE);

		printf("Recv: %s\n\n", recvBuffer);

		if (!strcmp(recvBuffer, SM_EXIT)) {
			printf("Disconnect...\n");
			break;
		}

		len = getFileContent(recvBuffer, &fileContentBuffer, SC_BUFFER_SIZE);

		if (len == -1)
			fileContentBuffer = (char*)"File not found!";
		
		sprintf_s(
			sendBuffer,
			SC_BUFFER_SIZE,
			"File: %s\nLength: %d\nContent:\n%s",
			recvBuffer,
			len,
			fileContentBuffer
		);

		sendData(&clientSock.first, sendBuffer);

		delete[] recvBuffer;
		delete[] fileContentBuffer;
		delete[] sendBuffer;
	}

	closesocket(serverSock);
	closesocket(clientSock.first);
	WSACleanup();

	printf("Exit...\n\n");

	return 0;
}