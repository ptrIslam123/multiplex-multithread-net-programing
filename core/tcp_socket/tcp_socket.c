#include "tcp_socket.h"

#include <string.h>
#include <errno.h>

TcpSocketResult acceptTcpConnection(const TcpSocket *const tcpSocket, AddressIn *const clientAddressIn,
                        socklen_t *const addressLen) {
    const int clientFd = accept(tcpSocket->socket, (struct sockaddr*)clientAddressIn, addressLen);
    if (clientFd < 0) {
        TcpSocketResult result;
        result.isSuccessful = -1;
        result.value.errorCode = errno;
        return result;
    }
    return makeTcpSocket(clientFd, 0);
}

int makeTcpConnectionQueue(const TcpSocket *const tcpSocket, const int capacityQueue) {
    if ((listen(tcpSocket->socket, capacityQueue)) < 0) {
        return -1;
    }
    return 0;
}

int bindTcpSocket(const TcpSocket *const tcpSocket) {
    if ((bind(tcpSocket->socket, (struct sockaddr*)&tcpSocket->address,
            sizeof(tcpSocket->address))) < 0) {
        return -1;
    }
    return 0;
}

TcpSocketResult makeTcpSocket(const int socketFd, const Port port) {
    TcpSocketResult result;
    const AddressIn address = makeTcpSocketAddress(port);
    int fd = socketFd;
    if (socketFd == 0) {
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            result.isSuccessful = -1;
            result.value.errorCode = errno;
            return result;
        }
    }
    const TcpSocket tcpSocket = {
            .socket = fd, .address = address, .port = port
    };
    result.isSuccessful = 0;
    result.value.tcpSocket = tcpSocket;
    return result;
}

AddressIn makeTcpSocketAddress(const Port port) {
    AddressIn socketAddressIn;
    memset(&socketAddressIn, 0, sizeof(socketAddressIn));
    socketAddressIn.sin_family = AF_INET;
    socketAddressIn.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddressIn.sin_port = htons(port);
    return socketAddressIn;
}