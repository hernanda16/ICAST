#ifndef MULTICAST_V2_HPP
#define MULTICAST_V2_HPP

#include "multicast_abstract.hpp"

class Multicast_v2 : public Multicast
{
private:
public:
    Multicast_v2() {}
    ~Multicast_v2() {}

    bool readyToSend()
    {
        printf("readyToSend V2 \n");
    }
    int send(std::vector<uint8_t> data, bool blocking = false)
    {
        printf("send V2\n");
    }
    int recv(std::vector<uint8_t> &data, bool blocking = false)
    {
        printf("recv V2\n");
    }
};

#endif