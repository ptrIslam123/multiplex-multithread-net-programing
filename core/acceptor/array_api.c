#include "array_api.h"

#include <string.h>

struct pollfd *backPollFd(TcpMultiplexAcceptor *const acceptor) {
    return &(acceptor->clientPollFdSet[acceptor->clientPollFdSetSize]);
}

int pushBackPollFd(TcpMultiplexAcceptor *const acceptor, const struct pollfd pollFd) {
    const int size = acceptor->clientPollFdSetSize;
    if (size + 1 > MAX_POSSIBLE_SIZE_TCP_SESSIONS) {
        return -1;
    }
    memcpy(backPollFd(acceptor), &pollFd, sizeof(pollFd));
    acceptor->clientPollFdSetSize = size + 1;
    return 0;
}

void popBackPollFd(TcpMultiplexAcceptor *const acceptor) {
    const int size = acceptor->clientPollFdSetSize;
    if (size > 0) {
        memset(acceptor->clientPollFdSet + size - 1, 0, sizeof(struct pollfd));
        --acceptor->clientPollFdSetSize;
    }
}

TcpSession *backTcpSessions(TcpMultiplexAcceptor *const acceptor) {
    return &(acceptor->tcpSessions[acceptor->tcpSessionsSize]);
}

int pushBackTcpSession(TcpMultiplexAcceptor *const acceptor, const TcpSession tcpSession) {
    const size_t size = acceptor->tcpSessionsSize;
    if (size + 1 > MAX_POSSIBLE_SIZE_TCP_SESSIONS) {
        return -1;
    }
    memcpy(backTcpSessions(acceptor), &tcpSession, sizeof(tcpSession));
    acceptor->tcpSessionsSize = size + 1;
    return 0;
}

void popBackTcpSession(TcpMultiplexAcceptor *const acceptor) {
    const int size = acceptor->tcpSessionsSize;
    if (size > 0) {
        memset(acceptor->tcpSessions + size - 1, 0, sizeof(TcpSession));
        --acceptor->tcpSessionsSize;
    }
}

void removePollFd(TcpMultiplexAcceptor *const acceptor, const int index) {
    if (index == acceptor->clientPollFdSetSize - 1) {
        popBackPollFd(acceptor);
        return;
    }

    for (int current = index, next = current + 1;
            next < acceptor->clientPollFdSetSize; ++current, ++next) {
        struct pollfd *currentPollFd = &acceptor->clientPollFdSet[current];
        struct pollfd *nextPollFd = &acceptor->clientPollFdSet[next];
        *currentPollFd = *nextPollFd;
    }
    popBackPollFd(acceptor);
}

void removeTcpSession(TcpMultiplexAcceptor *const acceptor, const int index) {
    if(index == acceptor->tcpSessionsSize - 1) {
        popBackTcpSession(acceptor);
        return;
    }

    for (int current = index, next = current + 1;
         next < acceptor->tcpSessionsSize; ++current, ++next) {
        TcpSession *currentPollFd = &acceptor->tcpSessions[current];
        TcpSession *nextPollFd = &acceptor->tcpSessions[next];
        *currentPollFd = *nextPollFd;
    }
    popBackTcpSession(acceptor);
}
