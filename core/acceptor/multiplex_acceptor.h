#ifndef PROJECT_MULTIPLEX_ACCEPTOR_H
#define PROJECT_MULTIPLEX_ACCEPTOR_H

#include <poll.h>

#include "tcp_socket.h"

#define MAX_POSSIBLE_SIZE_TCP_SESSIONS 5

struct TcpSession;
typedef void (*Callback)(struct TcpSession*, void*);

typedef enum {
    TcpSessionStatus_Open,
    TcpSessionStatus_Close
} TcpSessionStatus;

typedef struct {
    void *callbackData;
    Callback callback;
} RequestHandler;

typedef struct TcpSession {
    TcpSocket clientSocket;
    RequestHandler requestHandler;
    TcpSessionStatus status;
} TcpSession;

typedef struct {
    RequestHandler requestHandler;
    int tcpSessionsSize;
    int clientPollFdSetSize;
    struct pollfd clientPollFdSet[MAX_POSSIBLE_SIZE_TCP_SESSIONS];
    struct TcpSession tcpSessions[MAX_POSSIBLE_SIZE_TCP_SESSIONS];
} TcpMultiplexAcceptor;

#endif //PROJECT_MULTIPLEX_ACCEPTOR_H
