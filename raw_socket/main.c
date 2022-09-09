#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

#include "extract_headers.h"
#include "net_interface_info.h"
#include "maker_net_headers.h"

UDPPackageInfo getInfoForSendingUDPPackage(unsigned char *const buffer,
                                             const NetInterfaceInfo *const netInterfaceInfo) {
    const struct udphdr udphdr = extractUDPHeader(buffer);
    const struct iphdr iphdr = extractIpHeader(buffer);
    const struct ethhdr ethhdr = extractEthernetHeader(buffer);
    UDPPackageInfo udpPackageInfo;

    udpPackageInfo.sourcePort = udphdr.dest,
    udpPackageInfo.destinationPort = udphdr.source,
    udpPackageInfo.interfaceInfo = *netInterfaceInfo,

    memcpy(udpPackageInfo.destinationMacAddress.sa_data, ethhdr.h_source,
           sizeof(ethhdr.h_source));

    struct sockaddr_in destinationIPAddress;
    {
        struct in_addr destinationInAddress = {.s_addr = iphdr.saddr};
        const char *const destinationInAddressStr = inet_ntoa(destinationInAddress);
        if (destinationInAddressStr == NULL) {
            fprintf(stderr, "Can`t get Ip address string\n");
            exit(-1);
        }

        if (inet_pton(AF_INET, destinationInAddressStr, &destinationIPAddress.sin_addr) < 0) {
            fprintf(stderr, "Can`t get struct sockaddr by Ip address string\n");
            exit(-1);
        }
    }

    memcpy(udpPackageInfo.destinationIpAddress.sa_data,
           ((struct sockaddr*)&destinationIPAddress)->sa_data, sizeof(iphdr.saddr));
    return udpPackageInfo;
}

struct sockaddr_ll makeSocketAddressLL(unsigned char *const buffer,
        const NetInterfaceInfo *const netInterfaceInfo) {
    const struct ethhdr ethhdr = extractEthernetHeader(buffer);
    const struct iphdr iphdr = extractIpHeader(buffer);
    struct sockaddr_ll address;
    // net interface inode (index net device)
    address.sll_ifindex = netInterfaceInfo->inode;
    // MAC address length
    address.sll_halen = ETH_ALEN;
    // set address family
    address.sll_family = AF_INET;
    // set destination MAC address
    memcpy(address.sll_addr, ethhdr.h_source, sizeof(ethhdr.h_source));
    return address;
}

int main(int argc, char **argv) {
    /**
     * @brief Не уверен что программа работает. На данный момент это мой максимум
     * что могу сделать на таком низком уровне программирвания сокетов!!!
     */
    if (argc != 2) {
        fprintf(stderr, "No passing input argument: interface name\n");
        return EXIT_FAILURE;
    }

    const char *interfaceName = argv[1];
    int sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_UDP);
    if(sock < 0) {
        fprintf(stderr, "Can`t create row socket\n");
        return EXIT_FAILURE;
    }

    const NetInterfaceInfoResult result = getNetInterfaceInfo(interfaceName, sock);
    if (result.isSuccessful < 0) {
        fprintf(stderr, "Can`t get net interface info by name %s\n", interfaceName);
        return EXIT_FAILURE;
    }

    NetInterfaceInfo netInterfaceInfo = result.info;

    unsigned char buffer[10000] = {0};
    while (1) {
        const ssize_t receiveBufferSize = recv(sock, buffer, sizeof(buffer), 0);
        if (receiveBufferSize < 0) {
            fprintf(stderr, "system call read from socket was failed\n");
            break;
        }

        const UDPPackageInfo infoPackageForSending =
                getInfoForSendingUDPPackage(buffer, &netInterfaceInfo);
        const UDPPackageBuffer udpData = getUDPData(buffer, (size_t)receiveBufferSize);
        const unsigned char *const package = getUDPPackage(&infoPackageForSending, udpData,
                                                     (size_t)receiveBufferSize);

        struct sockaddr_ll address = makeSocketAddressLL(buffer, &netInterfaceInfo);
        const socklen_t addressLength = sizeof(address);
        if (sendto(sock, package, 64, 0, (const struct sockaddr*)&address, addressLength) < 0) {
            fprintf(stderr, "system call write was failed\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}