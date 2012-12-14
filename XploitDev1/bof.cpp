/*

Buffer Overflow Demo Program 

cl /GS- bof.cpp - Disable Stack cookie(/GS)
link /defaultlib:ws2_32.lib bof.obj 

*/
#include <winsock2.h>
#include <stdio.h>

#define BUFLEN 1024

void bof(char *recvbuf) {
	char sendBuf[512];
	strcpy(sendBuf,recvbuf);	
}

int main() {
	WSADATA wsaData;
	int result;
	sockaddr_in sock;
	SOCKET psk = INVALID_SOCKET;
	SOCKET csk = INVALID_SOCKET;

	char recvbuf[BUFLEN];		
	int recvResult, sendResult;
	int	recvbuflen = BUFLEN;

	result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup\n");
		return 1;
	}

	psk = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(psk == INVALID_SOCKET) {
		printf("Error at socket creation\n");
		WSACleanup();
		return 1;
	}

	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = inet_addr("0.0.0.0");
	sock.sin_port = htons(8888);

	result = bind(psk,(SOCKADDR*) &sock,sizeof(sock));
	if(result == SOCKET_ERROR) {
		printf("Bind failed\n");
		closesocket(psk);
		WSACleanup();
	}

	result = listen(psk,SOMAXCONN);
	if(result == SOCKET_ERROR) {
		printf("Listen failed\n");
		closesocket(psk);
		WSACleanup();
	}

	csk = accept(psk,NULL,NULL);
	if(psk == INVALID_SOCKET) {
		printf("Error at accept\n");
		WSACleanup();
		closesocket(psk);
		return 1;
	} else {
		printf("Client Connected\n");
	}

	do {
	memset(recvbuf,0,BUFLEN);
    	recvResult = recv(csk, recvbuf, BUFLEN, 0);
    	if (recvResult > 0) {   		
		printf("%s\n", recvbuf);
		bof(recvbuf);
        	sendResult = send(csk, recvbuf, recvResult, 0);
        	if (sendResult == SOCKET_ERROR) {
            		printf("Send failed: %d\n", WSAGetLastError());
            		closesocket(csk);
            		WSACleanup();
            		return 1;
        	}
        	printf("%s\n", recvbuf);
    	} else if (recvResult == 0) {
        	printf("Connection closing...\n");
	} else {
        	printf("Recieve failed: %d\n", WSAGetLastError());
        	closesocket(csk);
        	WSACleanup();
        	return 1;
    	}

	} while (recvResult > 0);

	return 0;
}
