#ifndef PROJECT_ARRAY_API_H
#define PROJECT_ARRAY_API_H

#include "multiplex_acceptor.h"

struct pollfd *backPollFd(TcpMultiplexAcceptor *acceptor);

int pushBackPollFd(TcpMultiplexAcceptor *acceptor, struct pollfd pollFd);

void popBackPollFd(TcpMultiplexAcceptor *acceptor);

TcpSession *backTcpSessions(TcpMultiplexAcceptor *acceptor);

int pushBackTcpSession(TcpMultiplexAcceptor *acceptor, TcpSession tcpSession);

void popBackTcpSession(TcpMultiplexAcceptor *acceptor);

void removePollFd(TcpMultiplexAcceptor *acceptor, int index);

void removeTcpSession(TcpMultiplexAcceptor *acceptor, int index);

#endif //PROJECT_ARRAY_API_H
