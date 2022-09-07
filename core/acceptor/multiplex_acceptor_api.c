#include "multiplex_acceptor_api.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "array_api.h"

void tcpListenerRequestHandler(TcpSession *const listenerSession, void *data) {
    TcpMultiplexAcceptor *const acceptor = (TcpMultiplexAcceptor*)data;
    const TcpSocketResult result = acceptTcpConnection(&listenerSession->clientSocket, NULL, NULL);
    if (result.isSuccessful < 0) {
        // TODO
    }

    {
        const TcpSocket clientSocket = result.value.tcpSocket;
        const struct pollfd clientPollFd = {.fd = clientSocket.socket, .events = POLLRDNORM};
        if (pushBackPollFd(acceptor, clientPollFd) < 0) {
            // TODO
        }
    }

    const RequestHandler clientRequestHandler = {
            .callbackData = acceptor->requestHandler.callbackData,
            .callback = acceptor->requestHandler.callback
    };
    const TcpSession clientTcpSession = {
            .clientSocket = result.value.tcpSocket,
            .requestHandler = clientRequestHandler,
            .status = TcpSessionStatus_Open
    };
    if (pushBackTcpSession(acceptor, clientTcpSession) < 0) {
        // TODO
    }
}

TcpMultiplexAcceptor *makeTcpMultiplexAcceptor(const TcpSocket *const tcpListener,
                                              Callback callback, void *const callbackData) {
    /**
     * @brief Пришлось выделять память под acceptor в heap, так как в
     * callback`е tcpListenerRequestHandler есть указатель на текущии acceptor.
     * Этот указатель всегда должен быть один и тот же, а при копирований указатель
     * внутри tcpListenerRequestHandler станет невалиден. Да и похорошему вместо malloc
     * лучше использовать кастомный аллокатор а не обычный.
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
    acceptor->clientPollFdSetSize = 0;
    const struct pollfd listenerPollFd = {.fd = tcpListener->socket, .events = POLLRDNORM};
    pushBackPollFd(acceptor, listenerPollFd);

    const RequestHandler listenerRequestHandler = {
            .callbackData = (void*)acceptor, .callback = tcpListenerRequestHandler
    };
    const TcpSession listenerTcpSession = {
            .clientSocket = *tcpListener,
            .requestHandler = listenerRequestHandler,
            .status = TcpSessionStatus_Open
    };
    pushBackTcpSession(acceptor, listenerTcpSession);
    return acceptor;
}

void handleReadySocket(TcpMultiplexAcceptor *const acceptor, const size_t socketIndex) {
    TcpSession *const curTcpSession = &(acceptor->tcpSessions[socketIndex]);
    const Callback const callback = curTcpSession->requestHandler.callback;
    TcpSocket *const clientSocket = &curTcpSession->clientSocket;
    callback(curTcpSession, curTcpSession->requestHandler.callbackData);
    if (curTcpSession->status == TcpSessionStatus_Close) {
        const int socketFd = clientSocket->socket;
        removePollFd(acceptor, socketIndex);
        removeTcpSession(acceptor, socketIndex);
        if (close(socketFd) < 0) {
            // TODO
        }
    }
}

void handleIOEvents(TcpMultiplexAcceptor *const acceptor, int countIOReadyFd) {
    for (int i = 0; countIOReadyFd > 0 && i < acceptor->clientPollFdSetSize; ++i) {
        const struct pollfd cur = acceptor->clientPollFdSet[i];
        if (cur.revents == POLLRDNORM) {
            handleReadySocket(acceptor, i);
            --countIOReadyFd;
        }
    }
}

void runEventPool(TcpMultiplexAcceptor *acceptor) {
    while (1) {
        const int result = poll(acceptor->clientPollFdSet, acceptor->tcpSessionsSize, -1);
        if (result < 0) {
            // TODO
        }
        handleIOEvents(acceptor, result);
    }
}

void destroyTcpMultiplexAcceptor(TcpMultiplexAcceptor *acceptor) {
    free(acceptor);
}