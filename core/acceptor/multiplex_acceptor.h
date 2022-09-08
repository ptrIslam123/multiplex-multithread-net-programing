#ifndef PROJECT_MULTIPLEX_ACCEPTOR_H
#define PROJECT_MULTIPLEX_ACCEPTOR_H

#include <poll.h>

#include "tcp_socket.h"

#define MAX_POSSIBLE_SIZE_TCP_SESSIONS 10000

typedef enum {
    TcpSessionStatus_Open,
    TcpSessionStatus_Close
} TcpSessionStatus;

typedef enum {
    MultiplexAcceptorStatus_Ok,
    MultiplexAcceptorStatus_AcceptError,
    MultiplexAcceptorStatus_IOError,
    MultiplexAcceptorStatus_SocketCloseError,
    MultiplexAcceptorStatus_ResourceExhaustion,
    MultiplexAcceptorStatus_BadAllocationMemory
} MultiplexAcceptorStatus;

struct TcpSession;
typedef void (*Callback)(struct TcpSession*, void*);

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
    char isStop;
    RequestHandler requestHandler;
    int tcpSessionsSize;
    int clientPollFdSetSize;
    struct pollfd clientPollFdSet[MAX_POSSIBLE_SIZE_TCP_SESSIONS];
    struct TcpSession tcpSessions[MAX_POSSIBLE_SIZE_TCP_SESSIONS];
} TcpMultiplexAcceptor;

#endif //PROJECT_MULTIPLEX_ACCEPTOR_H
