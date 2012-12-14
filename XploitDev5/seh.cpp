/*

SafeSEH Demo Program 

cl seh.cpp
link /SAFESEH /defaultlib:ws2_32.lib seh.obj

*/
#include <winsock2.h>
#include <stdio.h>

#define BUFLEN 2048

void getppr();

void seh(char *recvbuf) {
	char sendBuf[512];
	_try 
	{
		strcpy(sendBuf,recvbuf);	
		if(sendBuf[0] == '\x90') {
			   //Force an exception
			   int* p = 0x00000000;   
			   *p = 10;
		}	
	} 
	_except (EXCEPTION_EXECUTE_HANDLER) 
	{
		printf("Inside Exception Handler");
	}	    
}

int main() {
	getppr();
	WSADATA wsaData;
	int result;
	sockaddr_in sock;
	SOCKET psk = INVALID_SOCKET;
	SOCKET csk = INVALID_SOCKET;


	char *recvbuf = (char*) malloc(sizeof(char)*BUFLEN);		
	char *sendbuf = (char*) malloc(sizeof(char)*BUFLEN);		
	int recvResult, sendResult;
	int recvbuflen = BUFLEN;

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
	memset(sendbuf,0,BUFLEN);
    recvResult = recv(csk, recvbuf, BUFLEN, 0);
    if (recvResult > 0) {   		
		printf("%s\n", recvbuf);
		seh(recvbuf);
		gets(sendbuf);
		strcat(sendbuf,"\x0A");
        sendResult = send(csk, sendbuf, BUFLEN, 0);
        if (sendResult == SOCKET_ERROR) {
            printf("Send failed: %d\n", WSAGetLastError());
            closesocket(csk);
            WSACleanup();
            return 1;
        }        
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

void getppr() {
	__asm {
		mov esp,ebp
		sub esp,04
		pop ebp
		pop ebp
		retn
	}
}
