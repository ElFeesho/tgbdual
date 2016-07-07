/* 

multicast.c

The following program sends or receives multicast packets. If invoked
with one argument, it sends a packet containing the current time to an
arbitrarily chosen multicast group and UDP port. If invoked with no
arguments, it receives and prints these packets. Start it as a sender on
just one host and as a receiver on all the other hosts

*/
#include <iostream>
#include <string>
#include <thread>

#include "json.hpp"
#include "multicast_transmitter.h"

using nlohmann::json;

multicast_transmitter::multicast_transmitter(const std::string &group, uint16_t port) {
    mc_socket_in = socket(AF_INET, SOCK_DGRAM, 0);
    mc_socket_out = socket(AF_INET, SOCK_DGRAM, 0);

    memset((void *)&mc_sockaddr_in, 0, sizeof(struct sockaddr_in));
    memset((void *)&mc_sockaddr_out, 0, sizeof(struct sockaddr_in));

    mc_sockaddr_in.sin_family = mc_sockaddr_out.sin_family = AF_INET;

    mc_sockaddr_in.sin_port = mc_sockaddr_out.sin_port = htons(port);

    mc_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    mc_sockaddr_out.sin_addr.s_addr = inet_addr(group.c_str());

    if (bind(mc_socket_in, (struct sockaddr *)&mc_sockaddr_in, sizeof(mc_sockaddr_in)) < 0) {
        throw std::domain_error("Could not bind incoming multicast socket");
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(mc_socket_in, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        throw std::domain_error("Could not join multicast group");
    }
}

void multicast_transmitter::transcieve(std::function<void(std::string)> connectCallback) {
    std::string targetAddress;
    std::thread receiveThread{[&]() {
        while (shouldBroadcast) {
            char data[1024] = {0};
            socklen_t addrlen = 0;
            if (recvfrom(mc_socket_in, (void *)data, 1024, 0, (struct sockaddr *)&mc_sockaddr_in, &addrlen) < 0) {
                printf("Under-read from multicast address\n");
            } else {
                printf("Received: %s\n", data);
                auto payload = json::parse(data);

                std::string name = payload["name"];
                std::string addr = payload["addr"];
                std::string action = payload["action"];

                if (action == "startGame") {
                    std::cout << "Starting game with " << name << " on address " << addr << std::endl;
                    targetAddress = addr;
                    shouldBroadcast = false;
                }
            }
        }
    }};

    std::thread sendThread{[&]() {
        while (shouldBroadcast) {
            printf("Sending ping\n");
            std::string message{"{\"name\":\"Desktop\",\"addr\":\"10.10.2.240\",\"action\":\"announce\"}"};
            if (sendto(mc_socket_out, (void *)message.c_str(), message.length(), 0, (struct sockaddr *)&mc_sockaddr_out, sizeof(struct sockaddr)) != message.length()) {
                printf("Failed to send ping!\n");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }};

    receiveThread.join();
    sendThread.join();

    connectCallback(targetAddress);
}
