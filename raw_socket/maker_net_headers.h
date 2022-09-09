#ifndef PROJECT_MAKER_NET_HEADERS_H
#define PROJECT_MAKER_NET_HEADERS_H

#include "net_interface_info.h"
#include "extract_headers.h"

typedef struct {
    NetInterfaceInfo interfaceInfo;
    struct sockaddr destinationMacAddress;
    struct sockaddr destinationIpAddress;
    unsigned short destinationPort;
    unsigned short sourcePort;
} UDPPackageInfo;

unsigned char *getUDPPackage(const UDPPackageInfo *info, const UDPPackageBuffer udpData, size_t bufferSize);

#endif //PROJECT_MAKER_NET_HEADERS_H
