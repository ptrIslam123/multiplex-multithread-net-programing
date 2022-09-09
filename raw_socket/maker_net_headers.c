#include "maker_net_headers.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "reverse_buffer.h"

void fillEthernetHeaderInfo(const UDPPackageInfo *const info, unsigned char *buffer) {
    const size_t macAddressSize = sizeof(char) * 14;
    struct ethhdr *ethhdr = (struct ethhdr*)buffer;
    // set source ethernet address (For ethernet it is MAC address)
    memcpy(ethhdr->h_source, info->interfaceInfo.macAddress.sa_data, macAddressSize);

    // set destination ethernet address (For ethernet it is MAC address)
    memcpy(ethhdr->h_dest, info->destinationMacAddress.sa_data, macAddressSize);

    // set ethernet protocol
    ethhdr->h_proto = htons(ETH_P_IP);
}

void fillIpHeaderInfo(const UDPPackageInfo *const info, unsigned char *buffer) {
    struct iphdr *iphdr = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    iphdr->version = 4;
    iphdr->ihl = 5;
    iphdr->tos = 16;
    iphdr->id = htons(10201);
    iphdr->ttl = 64;
    iphdr->protocol = 17;

    // set destination address (For IP level it is IP address)
    struct in_addr inDestinationAddress =
            ((struct sockaddr_in*)&info->destinationIpAddress)->sin_addr;
    iphdr->daddr = inet_addr(inet_ntoa(inDestinationAddress));

    // set source address (For Ip level it is IP address)
    struct in_addr inSourceAddress =
            ((struct sockaddr_in*)&info->interfaceInfo.ipAddress)->sin_addr;
    iphdr->saddr = inet_addr(inet_ntoa(inSourceAddress));
}

void fillUDPHeaderInfo(const UDPPackageInfo *const info, unsigned char *buffer) {
    struct udphdr *udphdr = (struct udphdr*)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
    // set destination address (For transport level it is port number)
    udphdr->dest = info->destinationPort;

    // set source address (For transport level it is port number)
    udphdr->source = info->sourcePort;

    // set check sum
    udphdr->check = 0;

    // set UDP length
    udphdr->len = 0;
}

void fillUserData(unsigned char *const buffer, const unsigned char *const data, const size_t size) {
    reverseBuffer(buffer, size);
    memcpy(buffer, data, size);
}

void setHeaderLengthForIPAndUDP(unsigned char *const buffer, const size_t totalSize) {
    struct iphdr *iphdr = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    iphdr->tot_len = htons(totalSize - sizeof(struct ethhdr));

    struct udphdr *udphdr = (struct udphdr*)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
    udphdr->len = htons(totalSize - sizeof(struct iphdr) - sizeof(struct ethhdr));
}

unsigned short getCheckSum(unsigned char *const buffer, int _16bitword) {
    /**
     * @brief Метод вычисления контрольной сумы скопипасти из инета, так как не особо понял как его
     * вычислять!
     */
    unsigned long sum = 0;
    for (sum = 0; _16bitword > 0; --_16bitword) {
        sum += htons((*buffer)++);
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return sum;
}

unsigned char *getUDPPackage(const UDPPackageInfo *const info, const UDPPackageBuffer udpData, const size_t bufferSize) {
    size_t offset = 0;
    unsigned char *buffer = (unsigned char*) malloc(bufferSize);
    memset(buffer, 0, bufferSize);

    fillEthernetHeaderInfo(info, buffer);
    offset += sizeof(struct ethhdr);

    fillIpHeaderInfo(info, buffer);
    offset += sizeof(struct iphdr);

    fillUDPHeaderInfo(info, buffer);
    offset += sizeof(struct udphdr);

    fillUserData(buffer, udpData.data, udpData.size);
    offset += udpData.size;

    setHeaderLengthForIPAndUDP(buffer, offset);

    struct iphdr *iphdr = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    iphdr->check = getCheckSum((unsigned char*)(buffer + sizeof(struct ethhdr)),
            sizeof(struct iphdr) / 2);

    return buffer;
}