#include "net_interface_info.h"

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/if.h>

struct ifreq setNameIfreg(const char *name) {
    struct ifreq ifReg;
    memset(&ifReg, 0, sizeof(ifReg));
    strncpy(ifReg.ifr_ifrn.ifrn_name, name, IFNAMSIZ - 1);
    return ifReg;
}

int getInterfaceIndex(struct ifreq *const ifReg, const int socket) {
    if ((ioctl(socket, SIOCGIFINDEX, ifReg)) < 0) {
        return -1;
    }
    return 0;
}

int getInterfaceMacAddress(struct ifreq *const ifReg, const int socket) {
    if ((ioctl(socket, SIOCGIFHWADDR, ifReg)) < 0) {
        return -1;
    }
    return 0;
}

int getInterfaceIpAddress(struct ifreq *const ifReg, const int socket) {
    if ((ioctl(socket, SIOCGIFADDR, ifReg)) < 0) {
        return -1;
    }
    return 0;
}

NetInterfaceInfoResult getNetInterfaceInfo(const char *const interfaceName, const int socket) {
    NetInterfaceInfoResult result;
    result.isSuccessful = -1;
    NetInterfaceInfo info;
    struct ifreq inode = setNameIfreg(interfaceName);
    if (getInterfaceIndex(&inode, socket) < 0) {
        result.errorCode = errno;
        return result;
    }

    struct ifreq macAddress = setNameIfreg(interfaceName);
    if (getInterfaceMacAddress(&macAddress, socket) < 0) {
        result.errorCode = errno;
        return result;
    }

    struct ifreq ipAddress = setNameIfreg(interfaceName);
    if (getInterfaceIpAddress(&ipAddress, socket) < 0) {
        result.errorCode = errno;
        return result;
    }

    info.inode = inode.ifr_ifru.ifru_ivalue;
    info.macAddress = macAddress.ifr_ifru.ifru_hwaddr;
    info.ipAddress = ipAddress.ifr_ifru.ifru_addr;
    result.isSuccessful = 0;
    result.info = info;
    return result;
}