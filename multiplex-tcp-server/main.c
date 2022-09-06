#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core/tcp_socket/tcp_socket.h"
#include "core/acceptor/multiplex_acceptor.h"

void clientRequestHandler(const TcpSocket *clientSocket, void *data) {
    printf("Client request!!!\n");
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

    TcpMultiplexAcceptor *acceptor = makeTcpMultiplexAcceptor(&tcpSocket, 2, clientRequestHandler, NULL);
    runEventPool(acceptor);

    return 0;
}