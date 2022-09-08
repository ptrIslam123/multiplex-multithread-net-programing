#ifndef PROJECT_MULTIPLEX_ACCEPTOR_API_H
#define PROJECT_MULTIPLEX_ACCEPTOR_API_H

#include "multiplex_acceptor.h"

typedef struct {
    union {
        TcpMultiplexAcceptor *acceptor;
        MultiplexAcceptorStatus status;
    };
    char isSuccessful;
} InitTcpMultiplexAcceptorResult;

InitTcpMultiplexAcceptorResult makeTcpMultiplexAcceptor(const TcpSocket *tcpListener,
                                               Callback callback, void *callbackData);
void runEventPool(TcpMultiplexAcceptor *acceptor);
void destroyTcpMultiplexAcceptor(TcpMultiplexAcceptor *acceptor);

#endif //PROJECT_MULTIPLEX_ACCEPTOR_API_H
