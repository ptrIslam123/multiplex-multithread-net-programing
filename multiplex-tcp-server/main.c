#include <stdio.h>

#include "core/tcp_socket/tcp_socket.h"

int main() {
    printf("Hello world!\n");
    TcpSocket tcpSocket = makeTcpSocket(800);
    return 0;
}