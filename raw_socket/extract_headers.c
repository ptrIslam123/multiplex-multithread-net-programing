#include "extract_headers.h"

#include <string.h>

struct ethhdr extractEthernetHeader(void *buffer) {
    struct ethhdr *etHeaderStruct = (struct ethhdr *)(buffer);
    struct ethhdr result;
    memcpy(&result, etHeaderStruct, sizeof(struct ethhdr));
    return result;
}

struct iphdr extractIpHeader(void *buffer) {
    struct iphdr *iphdr = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct iphdr result;
    memcpy(&result, iphdr, sizeof(struct iphdr));
    return result;
}

struct udphdr extractUDPHeader(void *buffer) {
    struct iphdr iphdr = extractIpHeader(buffer);
    const unsigned int ipHeaderBytesLen = iphdr.ihl * 4;
    struct udphdr *udphdr = (struct udphdr*)(buffer + sizeof(struct ethhdr) + ipHeaderBytesLen);
    struct udphdr result;
    memcpy(&result, udphdr, sizeof(struct udphdr));
    return result;
}

UDPPackageBuffer getUDPData(void *const buffer, const size_t packageSize) {
    struct iphdr iphdr = extractIpHeader(buffer);
    const size_t ipHeaderBytesLen = (size_t)(iphdr.ihl * 4);
    const size_t offset = (ipHeaderBytesLen + sizeof(struct ethhdr) + sizeof(struct udphdr));
    const UDPPackageBuffer result = {
            .size = packageSize - offset, .data = (unsigned char*)(buffer + offset)
    };
    return result;
}