#include "multiplex_acceptor.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct pollfd *backPollFd(TcpMultiplexAcceptor *const acceptor) {
    return &(acceptor->clientPollFdSet[acceptor->clientPollFdSetSize]);
}

void pushBackPollFd(TcpMultiplexAcceptor *const acceptor, const struct pollfd pollFd) {
    const size_t size = acceptor->clientPollFdSetSize;
    if (size + sizeof(struct pollfd) > acceptor->tcpSessionsCapacity) {
        // TODO
    }
    memcpy(backPollFd(acceptor), &pollFd, sizeof(pollFd));
    acceptor->clientPollFdSetSize = size + 1;
}

void popBackPollFd(TcpMultiplexAcceptor *const acceptor) {

}

TcpSession *backTcpSessions(TcpMultiplexAcceptor *const acceptor) {
    return &(acceptor->tcpSessions[acceptor->tcpSessionsSize]);
}

void pushBackTcpSession(TcpMultiplexAcceptor *const acceptor, const TcpSession tcpSession) {
    const size_t size = acceptor->tcpSessionsSize;
    if (size + sizeof(struct pollfd) > acceptor->tcpSessionsCapacity) {
        // TODO
    }
    memcpy(backTcpSessions(acceptor), &tcpSession, sizeof(tcpSession));
    acceptor->tcpSessionsSize = size + 1;
}

void popBackTcpSession(TcpMultiplexAcceptor *const acceptor) {

}

void tcpListenerRequestHandler(const TcpSocket *const tcpListener, void *data) {
    TcpMultiplexAcceptor *const acceptor = (TcpMultiplexAcceptor*)data;
    const TcpSocketResult result = acceptTcpConnection(tcpListener, NULL, NULL);
    if (result.isSuccessful < 0) {

    }

    {
        const TcpSocket clientSocket = result.value.tcpSocket;
        const struct pollfd clientPollFd = {.fd = clientSocket.socket, .events = POLLRDNORM};
        pushBackPollFd(acceptor, clientPollFd);
    }

    RequestHandler clientRequestHandler = {
            .callbackData = NULL, .callback = acceptor->requestHandler.callback
    };
    TcpSession clientTcpSession = {
            .clientPollFd = backPollFd(acceptor),
            .clientSocket = result.value.tcpSocket,
            .requestHandler = clientRequestHandler
    };
    pushBackTcpSession(acceptor, clientTcpSession);
    printf("Add new client connection handler\n");
}

TcpMultiplexAcceptor *makeTcpMultiplexAcceptor(const TcpSocket *const tcpListener, size_t tcpSessionsSize,
                                              Callback callback, void *const callbackData) {
    /**
     * @brief Пришлось выделять память под acceptor в heap, так как в
     * callback`е tcpListenerRequestHandler есть указатель на текущий acceptor.
     * Этот указатель всегда должен быть один и тот же, а при копирований указатель
     * внутри tcpListenerRequestHandler станет невалиден.
     */
    TcpMultiplexAcceptor *acceptor = (TcpMultiplexAcceptor*)malloc(sizeof(TcpMultiplexAcceptor));
    if (acceptor == NULL) {
        // TODO
    }

    {
        const RequestHandler requestHandler = {
                .callback = callback, .callbackData = callbackData
        };
        acceptor->requestHandler = requestHandler;
    }

    acceptor->tcpSessionsSize = 0;
    acceptor->tcpSessionsCapacity = tcpSessionsSize;
    acceptor->clientPollFdSetSize = 0;
    acceptor->clientPollFdSetCapacity = tcpSessionsSize;

    const struct pollfd listenerPollFd = {.fd = tcpListener->socket, .events = POLLRDNORM};
    pushBackPollFd(acceptor, listenerPollFd);

    RequestHandler listenerRequestHandler = {
            .callbackData = (void*)acceptor, .callback = tcpListenerRequestHandler
    };
    TcpSession listenerTcpSession = {
            .clientPollFd = backPollFd(acceptor),
            .clientSocket = *tcpListener,
            .requestHandler = listenerRequestHandler
    };
    pushBackTcpSession(acceptor, listenerTcpSession);
    return acceptor;
}


void handleIOEvents(TcpMultiplexAcceptor *const acceptor, int countIOReadyFd) {
    for (int i = 0; countIOReadyFd > 0 && i < acceptor->clientPollFdSetSize; ++i) {
        const struct pollfd cur = acceptor->clientPollFdSet[i];
        if (cur.revents == POLLRDNORM) {
            const TcpSession *curTcpSession = &(acceptor->tcpSessions[i]);
            Callback const callback = curTcpSession->requestHandler.callback;
            callback(&curTcpSession->clientSocket, curTcpSession->requestHandler.callbackData);
            --countIOReadyFd;
        }
    }
}

void runEventPool(TcpMultiplexAcceptor *acceptor) {
    while (1) {
        const int result = poll(acceptor->clientPollFdSet, acceptor->tcpSessionsSize, -1);
        if (result < 0) {

        }
        handleIOEvents(acceptor, result);
    }
}