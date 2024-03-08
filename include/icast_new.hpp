#ifndef ICAST_NEW_HPP
#define ICAST_NEW_HPP

#include "multicast_v1.hpp"
#include "multicast_v2.hpp"

class Icast
{
private:
    Icast()
    {
    }

public:
    static Icast *getInstance()
    {
        static Icast instance_;
        return &instance_;
    }

    void operator=(const Icast &) = delete;
    Icast(Icast &other) = delete;
};

#endif