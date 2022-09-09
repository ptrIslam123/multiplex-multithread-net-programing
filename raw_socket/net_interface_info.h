#ifndef PROJECT_NET_INTERFACE_INFO_H
#define PROJECT_NET_INTERFACE_INFO_H

#include <sys/socket.h>

typedef struct {
    struct sockaddr macAddress;
    struct sockaddr ipAddress;
    int inode;
} NetInterfaceInfo;

typedef struct {
    union {
        NetInterfaceInfo info;
        int errorCode;
    };
    char isSuccessful;
} NetInterfaceInfoResult;

NetInterfaceInfoResult getNetInterfaceInfo(const char *interfaceName, int socket);

#endif //PROJECT_NET_INTERFACE_INFO_H
