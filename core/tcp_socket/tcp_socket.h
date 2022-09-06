#ifndef PROJECT_TCP_SOCKET_H
#define PROJECT_TCP_SOCKET_H

#include <netinet/in.h>

typedef unsigned short Port;
typedef struct sockaddr_in AddressIn;

typedef struct TcpSocket {
    int socket;
    AddressIn address;
    Port port;
} TcpSocket;

TcpSocket makeTcpSocket(Port port);
AddressIn makeTcpSocketAddress(Port port);
int bindTcpSocket(const TcpSocket *tcpSocket);
int makeTcpConnectionQueue(const TcpSocket *tcpSocket, int capacityQueue);

#endif //PROJECT_TCP_SOCKET_H
