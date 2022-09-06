#include "tcp_socket.h"

#include <string.h>

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

TcpSocket makeTcpSocket(const Port port) {
    const AddressIn address = makeTcpSocketAddress(port);
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {

    }

    const struct TcpSocket tcpSocket = {
            .socket = fd, .address = address, .port = port
    };
    return tcpSocket;
}

AddressIn makeTcpSocketAddress(const Port port) {
    AddressIn socketAddressIn;
    memset(&socketAddressIn, 0, sizeof(socketAddressIn));
    socketAddressIn.sin_family = AF_INET;
    socketAddressIn.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddressIn.sin_port = port;
    return socketAddressIn;
}