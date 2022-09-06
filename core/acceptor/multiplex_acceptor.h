#ifndef PROJECT_MULTIPLEX_ACCEPTOR_H
#define PROJECT_MULTIPLEX_ACCEPTOR_H

#include <poll.h>

#include "tcp_socket.h"

static const size_t MAX_POSSIBLE_SIZE_TCP_SESSIONS = 1000;
typedef void (*Callback)(const TcpSocket*, void*);

typedef struct {
    void *callbackData;
    Callback callback;
} RequestHandler;

typedef struct TcpSession {
    struct pollfd *clientPollFd;
    TcpSocket clientSocket;
    RequestHandler requestHandler;
} TcpSession;

typedef struct {
    RequestHandler requestHandler;
    size_t tcpSessionsSize;
    size_t tcpSessionsCapacity;
    size_t clientPollFdSetSize;
    size_t clientPollFdSetCapacity;
    struct pollfd clientPollFdSet[10];
    struct TcpSession tcpSessions[10];
} TcpMultiplexAcceptor;

TcpMultiplexAcceptor *makeTcpMultiplexAcceptor(const TcpSocket *tcpListener, size_t tcpSessionsSize, Callback callback, void *callbackData);
void runEventPool(TcpMultiplexAcceptor *acceptor);

#endif //PROJECT_MULTIPLEX_ACCEPTOR_H
