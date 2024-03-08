#ifndef MULTICAST_V1_HPP
#define MULTICAST_V1_HPP

#include "multicast_abstract.hpp"

class Multicast_v1 : public Multicast
{
private:
public:
    Multicast_v1() {}
    ~Multicast_v1() {}

    bool readyToSend()
    {
        printf("readyToSend V1\n");
    }
    int send(std::vector<uint8_t> data, bool blocking = false)
    {
        printf("send V1\n");
    }
    int recv(std::vector<uint8_t> &data, bool blocking = false)
    {
        printf("recv V1\n");
    }
};

#endif