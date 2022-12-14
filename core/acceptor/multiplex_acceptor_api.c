#include "multiplex_acceptor_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "array_api.h"

#define CLASS_NAME_FOR_LOGGER "TcpMultiplexAcceptor"

void tcpListenerRequestHandler(TcpSession *const listenerSession, void *data) {
    TcpMultiplexAcceptor *const acceptor = (TcpMultiplexAcceptor*)data;
    const TcpSocketResult result = acceptTcpConnection(&listenerSession->clientSocket, NULL, NULL);
    if (result.isSuccessful < 0) {
        acceptor->isStop = 1;
        fprintf(stderr, CLASS_NAME_FOR_LOGGER"::tcpListenerRequestHandler: "
               "Accept new connection is failed\n");
        return;
    }

    {
        const TcpSocket clientSocket = result.tcpSocket;
        const struct pollfd clientPollFd = {.fd = clientSocket.socket, .events = POLLRDNORM};
        if (pushBackPollFd(acceptor, clientPollFd) < 0) {
            fprintf(stderr, CLASS_NAME_FOR_LOGGER"::tcpListenerRequestHandler: "
                            "Can`t pushBack new client pollFd. Exhausted memory space with size %d\n",
                            MAX_POSSIBLE_SIZE_TCP_SESSIONS);
            return;
        }
    }

    const RequestHandler clientRequestHandler = {
            .callbackData = acceptor->requestHandler.callbackData,
            .callback = acceptor->requestHandler.callback
    };
    const TcpSession clientTcpSession = {
            .clientSocket = result.tcpSocket,
            .requestHandler = clientRequestHandler,
            .status = TcpSessionStatus_Open
    };
    if (pushBackTcpSession(acceptor, clientTcpSession) < 0) {
        fprintf(stderr, CLASS_NAME_FOR_LOGGER"::tcpListenerRequestHandler: "
                        "Can`t pushBack new client tcp session. Exhausted memory space with size %d\n",
                MAX_POSSIBLE_SIZE_TCP_SESSIONS);
        return;
    }
}

InitTcpMultiplexAcceptorResult makeTcpMultiplexAcceptor(const TcpSocket *const tcpListener,
                                              Callback callback, void *const callbackData) {
    /**
     * @brief ???????????????? ???????????????? ???????????? ?????? acceptor ?? heap, ?????? ?????? ??
     * callback`?? tcpListenerRequestHandler ???????? ?????????????????? ???? ?????????????? acceptor.
     * ???????? ?????????????????? ???????????? ???????????? ???????? ???????? ?? ?????? ????, ?? ?????? ?????????????????????? ??????????????????
     * ???????????? tcpListenerRequestHandler ???????????? ??????????????????. ???? ?? ???????????????????? ???????????? malloc
     * ?????????? ???????????????????????? ?????????????????? ?????????????????? ?? ???? ??????????????.
     */
    InitTcpMultiplexAcceptorResult result;
    result.isSuccessful = -1;

    TcpMultiplexAcceptor *acceptor = (TcpMultiplexAcceptor*)malloc(sizeof(TcpMultiplexAcceptor));
    if (acceptor == NULL) {
        result.status = MultiplexAcceptorStatus_BadAllocationMemory;
        return result;
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

    result.isSuccessful = 0;
    result.acceptor = acceptor;
    return result;
}

void handleReadySocket(TcpMultiplexAcceptor *const acceptor, const int socketIndex) {
    TcpSession *const curTcpSession = &(acceptor->tcpSessions[socketIndex]);
    const Callback const callback = curTcpSession->requestHandler.callback;
    TcpSocket *const clientSocket = &curTcpSession->clientSocket;
    callback(curTcpSession, curTcpSession->requestHandler.callbackData);
    if (curTcpSession->status == TcpSessionStatus_Close) {
        if (close(clientSocket->socket) < 0) {
            fprintf(stderr, CLASS_NAME_FOR_LOGGER"::handleReadySocket: "
                    "Can`t close file descriptor %d\n", clientSocket->socket);
        }
        removePollFd(acceptor, socketIndex);
        removeTcpSession(acceptor, socketIndex);
    }
}

void handleIOEvents(TcpMultiplexAcceptor *const acceptor, int countIOReadyFd) {
    for (int i = 0; countIOReadyFd > 0 && i < acceptor->clientPollFdSetSize; ++i) {
        const struct pollfd cur = acceptor->clientPollFdSet[i];
        if (cur.revents == POLLRDNORM) {
            --countIOReadyFd;
            handleReadySocket(acceptor, i);
        }
    }
}

void runEventPool(TcpMultiplexAcceptor *acceptor) {
    while (!acceptor->isStop) {
        const int result = poll(acceptor->clientPollFdSet, acceptor->tcpSessionsSize, -1);
        if (result < 0) {
            fprintf(stderr, CLASS_NAME_FOR_LOGGER"::runEventPool: "
                 "system call poll in event loop is failed with error code %d\n", result);
            break;
        }
        handleIOEvents(acceptor, result);
    }
}

void destroyTcpMultiplexAcceptor(TcpMultiplexAcceptor *acceptor) {
    free(acceptor);
}