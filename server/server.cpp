#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

//custom header files
#include "http.h"

//predefined max buffer size
#define BUFF_SIZE 5012

//initiation
SOCKET sock, client;

char buffer[BUFF_SIZE];

//Client struct, info about connected client
struct _client {
    bool con;
    sockaddr_in addr;
    SOCKET cs;
    int i;
};

//Error handling second step
void s_handle(int s)
{
	if(sock)
	   closesocket(sock);
	
    if(client)
	   closesocket(client);
	WSACleanup();
	printf("\nEXIT SIGNAL: %d\n", s);
	exit(0);
}

//Error handling first step
void s_cl(char * a, int x) 
{
	std::cout <<a;
	s_handle(x+1000);
}

//Disconnect client
void disconnect(_client *x) {
    if (x->cs) {
        closesocket(x->cs);
    }
    x->con = false;
    x->i = -1;
    char* client_ip_address = inet_ntoa(x->addr.sin_addr);
    std::cout << "\nClient at: " << client_ip_address << " disconnected. " <<std::endl;
    
}

//Accept client connection request
int accept(_client *x) {
    //if (x->con == false) {
        x->i = sizeof(sockaddr);
        x->cs = accept(sock, (sockaddr*) &x->addr, &x->i);
        if (x->cs != 0 && x->cs != SOCKET_ERROR) {
        std::cout << "\nSocket accepted!" << std::endl;

        x->con = true;
        return (true);
        }
    return (false);
}

//recieve data from client
int recieve(_client *x) {
    if (x->con == true) {

        std::cout << "\nsocket recieving..." << std::endl;

        x->i = recv(x->cs, buffer, BUFF_SIZE, 0);

        if (x->i == 0 || x->i < 0) {
            disconnect(x);
            std::fill(buffer, buffer+BUFF_SIZE, 0);
            return (false);

        }
        else if (x->i > 0) {
            std::cout << "\nRECIEVED DATA:\n{" << buffer << "\n}" << std::endl;
            return (true);
        }

    }
    return (false);
}

int send(_client *x) {
    if (x->con == true) {
        
        http_req request;
        http_resp resp;

        //location of page files
        char directory[] = "\\pages";

        //std::cout << "processing request...\n";

        if (process_req(&request, &resp, buffer) != true) {

            std::cout << "http construction error" << std::endl;
            return(false);
        }

        //std::cout << "sending data\n";
        x->i = send(x->cs, buffer, BUFF_SIZE, 0);
        if (x->i == 0 || x->i < 0 ) {
            disconnect(x);
        }
        std::cout << "\nSENT DATA:\n{" << buffer << "\n}" << std::endl;
        
        return (true);
    }

    return (false);
}


int main() {
    //SETUP
    int res, i = 1, port = 3000;

    WSAData data;

    addrinfo hints, *result = NULL, *ptr = NULL;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    _client client;

    //INIT
    std::cout << "Server (Multiple clients)." << std::endl;;

    res = WSAStartup(MAKEWORD(2,2), &data); 
    if (res != 0) {
        s_cl("\nWSAstartup failed", WSAGetLastError());
    };

    std::cout << "\nversion: " << data.wVersion
              << "\ndescription: " <<data.szDescription
              << "\nstatus: " << data.szSystemStatus <<"\n" << std::endl;


    res = getaddrinfo(NULL, "3000", &hints, &result);
    if (res!= 0) {
        s_cl("getaddrinfo failed", res);
    }
    
    for(ptr=result; ptr != NULL; ptr = ptr->ai_next) {
        std::cout << inet_ntoa(((sockaddr_in * ) ptr->ai_addr)->sin_addr) << std::endl;
    }

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sock == INVALID_SOCKET) {
            s_cl("\nInvalid socket", WSAGetLastError());
        }

        else if (sock == SOCKET_ERROR) {
            s_cl("\nSocket error", WSAGetLastError());
        }

        else {
            std::cout << "SOCKET established" << std::endl;
        }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i)); //able to reuse address
    
    res = bind(sock, result->ai_addr, result->ai_addrlen);
        if (res == SOCKET_ERROR) {
            s_cl("\nbind failed", WSAGetLastError());
        }

        else {
            std::cout << "\nSOCKET bound to port: " << port << std::endl;
        }

    freeaddrinfo(result);

    res = listen(sock, 5);
    
    if (res == SOCKET_ERROR) {
        s_cl("Listen failed", WSAGetLastError());
    } else {
        std::cout << "\nserver in listening mode" << std::endl;
    }

    unsigned long b=1;
    ioctlsocket(sock, FIONBIO, &b); //non blocking mode for socket

    //main loop
    while (sock) {
        if (accept(&client)) {
            
            if (recieve(&client)) {

                send(&client);
            }
        }

    }
    WSACleanup();
    return 0;
}


/*
Listen socket always active.
Process each request, end connection.
Process next in que.
*/