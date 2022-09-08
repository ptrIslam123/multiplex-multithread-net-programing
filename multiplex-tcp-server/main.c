#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "core/tcp_socket/tcp_socket.h"
#include "core/acceptor/multiplex_acceptor_api.h"

MultiplexAcceptorStatus clientRequestHandler(TcpSession *const session, void *data) {
    char buff[1024] = {0};
    int socket = session->clientSocket.socket;
    const ssize_t receiveBufferSize = read(socket, buff, sizeof(buff));
    if (receiveBufferSize < 0) {
        fprintf(stderr, "system call read is failed with error code %d\n", (int)receiveBufferSize);
        return MultiplexAcceptorStatus_IOError;
    }

    if (receiveBufferSize == 0) {
        session->status = TcpSessionStatus_Close;
        return MultiplexAcceptorStatus_Ok;
    }

    ssize_t sendBuffSize = 0;
    while ((sendBuffSize = write(socket, buff, receiveBufferSize)) < sendBuffSize) {
    }
    return MultiplexAcceptorStatus_Ok;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "No passing input argument: port number\n");
        return EXIT_FAILURE;
    }

    const TcpSocketResult tcpSocketResult = makeTcpSocket(0, (Port)atoi(argv[1]));
    if (tcpSocketResult.isSuccessful < 0) {
        fprintf(stderr, "Can`t create ctp socket listener\n");
        return EXIT_FAILURE;
    }

    const TcpSocket tcpSocket = tcpSocketResult.tcpSocket;
    if ((bindTcpSocket(&tcpSocket)) < 0) {
        fprintf(stderr, "Can`t bind socket address\n");
        return EXIT_FAILURE;
    }

    if ((makeTcpConnectionQueue(&tcpSocket, 5)) < 0) {
        fprintf(stderr, "Can`t create queue for incoming connection\n");
        return EXIT_FAILURE;
    }

    InitTcpMultiplexAcceptorResult resultMultiplexAcceptor =
            makeTcpMultiplexAcceptor(&tcpSocket, clientRequestHandler, NULL);
    if (resultMultiplexAcceptor.isSuccessful < 0) {
        fprintf(stderr, "Initialize Tcp multiplex tcp acceptor is failed\n");
        return EXIT_FAILURE;
    }

    TcpMultiplexAcceptor *const acceptor = resultMultiplexAcceptor.acceptor;
    runEventPool(acceptor);
    destroyTcpMultiplexAcceptor(acceptor);
    return EXIT_SUCCESS;
}