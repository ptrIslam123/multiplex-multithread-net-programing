#ifndef PROJECT_EXTRACT_HEADERS_H
#define PROJECT_EXTRACT_HEADERS_H

#include <linux/if_ether.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

struct ethhdr extractEthernetHeader(void *buffer);

struct iphdr extractIpHeader(void *buffer);

struct udphdr extractUDPHeader(void *buffer);

typedef struct {
    unsigned char *data;
    size_t size;
} UDPPackageBuffer;

UDPPackageBuffer getUDPData(void *buffer, size_t packageSize);

#endif //PROJECT_EXTRACT_HEADERS_H
