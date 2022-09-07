#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "core/tcp_socket/tcp_socket.h"
#include "core/acceptor/multiplex_acceptor_api.h"
#include "core/acceptor/array_api.h"

void clientRequestHandler(TcpSession *const session, void *data) {
    char buff[1024] = {0};
    int socket = session->clientSocket.socket;
    const ssize_t responseBuffSize = read(socket, buff, sizeof(buff));
    if (responseBuffSize < 0) {
        // TODO
    }

    if (responseBuffSize == 0) {
        session->status = TcpSessionStatus_Close;
    }

    ssize_t sendBuffSize = 0;
    while ((sendBuffSize = write(socket, buff, responseBuffSize)) < sendBuffSize) {
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        // TODO
    }

    TcpSocketResult result = makeTcpSocket(0, (Port)atoi(argv[1]));
    if (result.isSuccessful < 0) {
        // TODO
    }

    const TcpSocket tcpSocket = result.value.tcpSocket;
    if ((bindTcpSocket(&tcpSocket)) < 0) {
        // TODO
    }

    if ((makeTcpConnectionQueue(&tcpSocket, 5)) < 0) {
        // TODO
    }

    TcpMultiplexAcceptor *acceptor = makeTcpMultiplexAcceptor(&tcpSocket, clientRequestHandler, NULL);
    runEventPool(acceptor);
    destroyTcpMultiplexAcceptor(acceptor);
    return 0;
}