// Traceroute.cpp
#include "Traceroute.h"

u_short Traceroute::calculateChecksum(u_short* buffer, int size) {
    unsigned long sum = 0;
    while (size > 1) {
        sum += *buffer++;
        size -= 2;
    }
    if (size) sum += *(u_char*)buffer;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return static_cast<u_short>(~sum);
}

std::string Traceroute::resolveHostname(const in_addr& addr) {
    char host[NI_MAXHOST];
    sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_addr = addr;

    if (getnameinfo((sockaddr*)&sa, sizeof(sa), host, sizeof(host), NULL, 0, 0) == 0) {
        return std::string(host);
    }
    return "";
}

void Traceroute::Run() {
    SetConsoleOutputCP(CP_UTF8);
    std::string input;
    std::cout << "Enter the IP or domain: ";
    std::cin >> input;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket error: " << WSAGetLastError() << "\n";
        return;
    }

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, input.c_str(), &destAddr.sin_addr) != 1) {
        hostent* h = gethostbyname(input.c_str());
        if (!h) {
            std::cerr << "[!] Failed to resolve the address\n";
            return;
        }
        destAddr.sin_addr = *(in_addr*)h->h_addr;
    }

    std::cout << "\nTracing route to " << inet_ntoa(destAddr.sin_addr) << "...\n";
    std::cout << std::setw(3) << "TTL" << "  "
        << std::setw(50) << "Hostname"
        << std::setw(18) << "IP"
        << "   [ms1  ms2  ms3]\n";

    for (int ttl = 1; ttl <= 30; ++ttl) {
        setsockopt(sock, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));

        std::string hopIp = "", hopHost = "";
        std::vector<int> times(3, -1);
        bool reached = false;

        for (int i = 0; i < 3; ++i) {
            char sendBuf[32] = { 0 };
            sendBuf[0] = 8; // Echo Request
            sendBuf[1] = 0;
            *(u_short*)(sendBuf + 4) = htons(1234);
            *(u_short*)(sendBuf + 6) = htons(ttl * 3 + i);
            *(u_short*)(sendBuf + 2) = 0;
            *(u_short*)(sendBuf + 2) = calculateChecksum((u_short*)sendBuf, sizeof(sendBuf));

            sockaddr_in from{};
            int fromLen = sizeof(from);
            char recvBuf[1024];

            auto start = std::chrono::high_resolution_clock::now();
            sendto(sock, sendBuf, sizeof(sendBuf), 0, (sockaddr*)&destAddr, sizeof(destAddr));

            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(sock, &fds);
            timeval timeout{ 2, 0 };

            int ready = select(0, &fds, NULL, NULL, &timeout);
            if (ready > 0) {
                int recvLen = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&from, &fromLen);
                if (recvLen >= 28) {
                    u_char type = recvBuf[20];
                    u_char code = recvBuf[21];

                    auto end = std::chrono::high_resolution_clock::now();
                    int ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                    hopIp = inet_ntoa(from.sin_addr);
                    times[i] = ms;

                    if (type == 0 && code == 0 && from.sin_addr.s_addr == destAddr.sin_addr.s_addr) {
                        reached = true;
                    }
                }
            }
        }

        std::cout << std::setw(3) << ttl << "  ";

        if (!hopIp.empty()) {
            in_addr addr{};
            addr.s_addr = inet_addr(hopIp.c_str());
            hopHost = resolveHostname(addr);
            std::cout << std::setw(50) << (hopHost.empty() ? "-" : hopHost)
                << std::setw(18) << hopIp << "   ";
        }
        else {
            std::cout << std::setw(50) << "*"
                << std::setw(18) << "*"
                << "   ";
        }

        for (int t : times) {
            if (t >= 0) std::cout << std::setw(4) << t << " ";
            else std::cout << "  *  ";
        }

        std::cout << "\n";

        if (reached) {
            std::cout << "\n[*] Objective achieved\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
}