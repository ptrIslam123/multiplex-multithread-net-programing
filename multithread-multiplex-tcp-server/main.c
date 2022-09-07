#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "core/acceptor/multiplex_acceptor_api.h"
#include "core/thread_pool/static_thread_pool_api.h"

void taskHandler(void *arg) {
    TcpSession *const session = (TcpSession*)arg;
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

void clientRequestHandler(TcpSession *const session, void *data) {
    StaticThreadPool *const pool = (StaticThreadPool*)data;
    const WorkerContext context = {
            .isStop = 0,
            .callbackContext = session,
            .callback = taskHandler
    };
    submitToStaticThreadPool(pool, context);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        // TODO
    }

    InitStaticThreadPoolResult poolInitResult = makeStaticThreadPool(4);
    if (poolInitResult.isSuccessful < 0) {
        // TODO
    }
    StaticThreadPool threadPool = poolInitResult.pool;
    TcpSocketResult result = makeTcpSocket(0, (Port)atoi(argv[1]));
    if (result.isSuccessful < 0) {
        // TODO
    }

    const TcpSocket tcpSocket = result.tcpSocket;
    if ((bindTcpSocket(&tcpSocket)) < 0) {
        // TODO
    }

    if ((makeTcpConnectionQueue(&tcpSocket, 5)) < 0) {
        // TODO
    }

    TcpMultiplexAcceptor *acceptor =
            makeTcpMultiplexAcceptor(&tcpSocket, clientRequestHandler, (void*)&threadPool);
    runEventLoopStaticThreadPool(&threadPool);
    runEventPool(acceptor);
    destroyTcpMultiplexAcceptor(acceptor);
    joinToStaticThreadPool(&threadPool);
    return 0;
}