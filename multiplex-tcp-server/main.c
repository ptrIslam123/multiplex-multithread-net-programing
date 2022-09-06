#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core/tcp_socket/tcp_socket.h"

void handleTcpConnection(TcpSocket *const tcpSocket) {
    const char msg[] = "Hello world";
    write(tcpSocket->socket, msg, strlen(msg));
}

int main() {
    TcpSocketResult result = makeTcpSocket(0, 8000);
    if (result.isSuccessful < 0) {

    }

    const TcpSocket tcpSocket = result.value.tcpSocket;
    if ((bindTcpSocket(&tcpSocket)) < 0) {

    }

    if ((makeTcpConnectionQueue(&tcpSocket, 5)) < 0) {

    }
    AddressIn clientAddressIn;
    memset(&clientAddressIn, 0, sizeof(clientAddressIn));
    socklen_t clientAddressLen = sizeof(clientAddressIn);
    while (1) {
        result = acceptTcpConnection(&tcpSocket, &clientAddressIn, &clientAddressLen);
        if (result.isSuccessful < 0) {

        }
        printf("New client connection\n");
        handleTcpConnection(&result.value.tcpSocket);
    }

    return 0;
}