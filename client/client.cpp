#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

 #define DEFAULT_BUFLEN 512
 #define DEFAULT_PORT "3000"


int _cdecl main(int argc, char **argv) {
    
    WSAData wsaData;

    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int iResult;


    //validate args
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[1]);
        return 1;
    }

    //init winsocket
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("startup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //resolve server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    
    //creating socket
    for (ptr=result; ptr->ai_addr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == SOCKET_ERROR) {
            printf("Error at socket(): %d\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        //connecting to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("address invalid: %d\n", ptr->ai_addr);
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    //send initial buffer
    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("bytes sent: %d\n", iResult);

    //shutdown sending side of connection, no more data to send
    //still able to recieve
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    //continue recieving data from server until peer shuts down
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            char recv_msg[iResult];
            for (int i = 0; i < iResult ; i++) {
                recv_msg[i] = (char)recvbuf[i];
            }

            printf("bytes recieved: %d\n", iResult);
            printf("recv: %d\n", recv_msg);
        } else if (iResult == 0) {
            printf("connetion closed\n"); 
        
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
        }

    } while (iResult > 0);
    
    //cleanup
    closesocket(ConnectSocket);
    WSACleanup();
    
    return 0;
}