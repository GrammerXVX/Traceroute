#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

class Traceroute {
public:
    void Run();

private:
    u_short calculateChecksum(u_short* buffer, int size);
    std::string resolveHostname(const in_addr& addr);
};