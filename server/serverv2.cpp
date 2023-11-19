#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "files.h"
#include "http_request.h"

#define MAX_CONS 5
#define BUFF_SIZE 512
int PR_CONS;

SOCKET sock, client;

char buffer[BUFF_SIZE];

struct _client {
    bool con;
    bool msg;
    sockaddr_in addr;
    SOCKET cs;
    fd_set set;
    int i;
};

//Clients, num connections accepted
_client clients[MAX_CONS];

LPWIN32_FIND_DATAA findData;
//Error handling functions
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

void s_cl(char * a, int x) 
{
	std::cout <<a;
	s_handle(x+1000);
}
/*
void s_cl(char * a, int x, bool h) 
{
	std::cout <<a;
}*/

//Server functions
int accept(_client *x) {
    x->i = sizeof(sockaddr);
    x->cs = accept(sock, (sockaddr *) &x->addr, &x->i);
    if (x->cs != 0 && x->cs != SOCKET_ERROR) {
        std::cout << "\nconnecting.." <<std::endl;

        x->con = true;
        FD_ZERO(&x->set);
        FD_SET(x->cs, &x->set);
        return ( true );
    }
    return ( false );
}

void disconnect(_client *x) {
    if (x->cs) {
        closesocket(x->cs);
    }
    x->con = false;
    x->i = -1;
    PR_CONS --;
    char* client_ip_address = inet_ntoa(x->addr.sin_addr);
    std::cout << "\nclient at: " << client_ip_address << " disconnected. " << PR_CONS <<std::endl;
    
}

int send(_client *x, int sz) {

    if (x->con == true) {
        char payload_data[BUFF_SIZE];
        char file_name[] = "pages/index.html";
        size_t bytes;
        
        read(file_name, payload_data, &bytes);
        
        //std::cout << payload_data << "\n" << std::endl;
        x->i = send(x->cs, payload_data, sz, 0);
        x->msg = false; //no error correction this way, msg only sent once
        if (x->i == 0 || x->i == SOCKET_ERROR) {
            disconnect(x);
            return(false);
        }
       
        else return (true);
    }

    return (false);
}

int recv(_client *x, char *buffer, int sz) {
    if (FD_ISSET(x->cs, &x->set)) {
        x->i = recv(x->cs, buffer, sz, 0);
        
        if (x->i == 0 || x->i == SOCKET_ERROR) {
            disconnect(x);
            return (false);
        }

        else if (x->i > 0) {
            x->msg = true;
            
            return (true);
        }
       
    }
    return (false);

}

void accept_clients() {
    for (int i = 0; i < MAX_CONS; i++) {
        if (!clients[i].con) {
            if (accept(&clients[i])) {
                PR_CONS ++;
                char* client_ip_address = inet_ntoa(clients[i].addr.sin_addr);
                std::cout << "\naccepting client to array-position: " << i
                          << " with ip: " << client_ip_address << std::endl;
            }
            
        }
    }
}

void recv_clients() {
    for (int i = 0; i < MAX_CONS; i++) {
        if (clients[i].con) {
            if (recv(&clients[i], buffer, BUFF_SIZE)) {
            std::cout << "\ndata: {" << buffer << "\n} from: " << i << std::endl;
            }
        }   
    }
}

void send_clients() {
    for(int i = 0; i < MAX_CONS; i++) {
        if (clients[i].msg) {
            if (send(&clients[i], BUFF_SIZE)) {
            std::cout << "\nresponse sent to: " << inet_ntoa(clients[i].addr.sin_addr) <<std::endl;
            }
        }
    }
}

int main() {
    //SETUP
    int res, i = 1, port = 3000;

    WSAData data;

    /*
    sockaddr_in server;
    sockaddr addr;
    server.sin_family = AF_INET;
    server.sin_port = htons(port); 
    server.sin_addr.s_addr = INADDR_ANY;

    memcpy(&addr, &server, sizeof(SOCKADDR_IN));
    */

    addrinfo hints, *result = NULL, *ptr = NULL;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


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
        std::cout << "\nserver listening mode : " << res << std::endl;
    }
    
    unsigned long b=1;
    ioctlsocket(sock, FIONBIO, &b); //non blocking mode for socket

    while (true) {
        accept_clients();
        recv_clients();
        send_clients();
    }

    WSACleanup();
    return 0;
}