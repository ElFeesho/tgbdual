#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

class multicast_transmitter {
   public:
    multicast_transmitter(const std::string &group, uint16_t port);

    void transcieve(std::function<void(std::string)> connectCallback);

   private:
    int mc_socket_out;
    int mc_socket_in;

    struct sockaddr_in mc_sockaddr_out;
    struct sockaddr_in mc_sockaddr_in;

    bool shouldBroadcast{true};

    std::string ip;
};