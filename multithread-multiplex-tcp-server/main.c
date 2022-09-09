#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "core/acceptor/multiplex_acceptor_api.h"
#include "core/thread_pool/static_thread_pool_api.h"
#include "core/utils/reverse_buffer/reverse_buffer.h"

struct revers {};
void taskHandler(void *arg) {
    TcpSession *const session = (TcpSession*)arg;
    /**
     * @brief Даная проверка нужна из-за возможной рассихронизации acceptor`а и очереди задач.
     * В процессе обработки запроса worker`ом если обнаруживает что сокет закрыт то устанавливает
     * соответствующий флаг и acceptor проверяет этот флаг после отработки колбека, и если
     * после обработки acceptor видит что выставлен флаг указывающий что конекшин ассоциированный
     * с эти сокетом закрыт, закрывает этот сокет. Но данная модель хорошо работате в однопоточной
     * среде. В данном случае обработчик запроса кладется в отчередь и она не сразу будет обработана,
     * соответственно acceptor может быть не извещен о том что конект был разорван и не закрыть сокет сразу.
     */
    if (session->status == TcpSessionStatus_Close) {
        return;
    }

    char buff[1024] = {0};
    int socket = session->clientSocket.socket;
    const ssize_t receiveBufferSize = read(socket, buff, sizeof(buff));
    if (receiveBufferSize < 0) {
        fprintf(stderr, "system call read is failed with error code %d for file descriptor %d\n",
                (int)receiveBufferSize, socket);
        return;
    }

    if (receiveBufferSize == 0) {
        session->status = TcpSessionStatus_Close;
        return;
    }

    reverseBuffer((unsigned char*)buff, receiveBufferSize);
    ssize_t sendBuffSize = 0;
    while ((sendBuffSize = write(socket, buff, receiveBufferSize)) < sendBuffSize) {}
}

void clientRequestHandler(TcpSession *const session, void *data) {
    StaticThreadPool *const pool = (StaticThreadPool*)data;
    const WorkerContext context = {
            .isStop = 0,
            .callbackContext = session,
            .callback = taskHandler
    };
    submitToStaticThreadPool(pool, context);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "No passing input argument: port number\n");
        return EXIT_FAILURE;
    }

    const TcpSocketResult tcpSocketResult = makeTcpSocket(0, (Port)atoi(argv[1]));
    if (tcpSocketResult.isSuccessful < 0) {
        fprintf(stderr, "Can`t create ctp socket listener\n");
        return EXIT_FAILURE;
    }

    const TcpSocket tcpSocket = tcpSocketResult.tcpSocket;
    if ((bindTcpSocket(&tcpSocket)) < 0) {
        fprintf(stderr, "Can`t bind socket address\n");
        return EXIT_FAILURE;
    }

    if ((makeTcpConnectionQueue(&tcpSocket, 5)) < 0) {
        fprintf(stderr, "Can`t create queue for incoming connection\n");
        return EXIT_FAILURE;
    }

    InitStaticThreadPoolResult poolInitResult = makeStaticThreadPool(4);
    if (poolInitResult.isSuccessful < 0) {
        fprintf(stderr, "Initialize static thread pool is failed\n");
        return EXIT_FAILURE;
    }
    StaticThreadPool threadPool = poolInitResult.pool;
    runEventLoopStaticThreadPool(&threadPool);

    InitTcpMultiplexAcceptorResult resultMultiplexAcceptor =
            makeTcpMultiplexAcceptor(&tcpSocket, clientRequestHandler, (void*)&threadPool);
    if (resultMultiplexAcceptor.isSuccessful < 0) {
        fprintf(stderr, "Initialize Tcp multiplex tcp acceptor is failed\n");
        return EXIT_FAILURE;
    }
    TcpMultiplexAcceptor *const acceptor = resultMultiplexAcceptor.acceptor;
    runEventPool(acceptor);
    joinToStaticThreadPool(&threadPool);

    destroyTcpMultiplexAcceptor(acceptor);
    destroyTcpMultiplexAcceptor(acceptor);
    return EXIT_SUCCESS;
}