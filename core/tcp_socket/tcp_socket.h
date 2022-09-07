#ifndef PROJECT_TCP_SOCKET_H
#define PROJECT_TCP_SOCKET_H

#include <netinet/in.h>

typedef unsigned short Port;
typedef struct sockaddr_in AddressIn;

typedef struct {
    int socket;
    AddressIn address;
    Port port;
} TcpSocket;

typedef struct {
    union {
        int errorCode;
        TcpSocket tcpSocket;
    };

    char isSuccessful;
    union value;
} TcpSocketResult;

TcpSocketResult makeTcpSocket(int socketFd, Port port);
AddressIn makeTcpSocketAddress(Port port);
int bindTcpSocket(const TcpSocket *tcpSocket);
int makeTcpConnectionQueue(const TcpSocket *tcpSocket, int capacityQueue);
TcpSocketResult acceptTcpConnection(const TcpSocket *tcpSocket, AddressIn *clientAddressIn,
                         socklen_t *addressLen);

#endif //PROJECT_TCP_SOCKET_H
