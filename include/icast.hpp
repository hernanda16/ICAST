#ifndef ICAST_HPP
#define ICAST_HPP

#include "../include/dictionary.hpp"
// #include "../include/multicast.hpp"
// #include "../include/multicast2.hpp"
#include "../include/multicast3.hpp"

#define MC_LOOPBACK 0

class Icast {
private:
    Icast()
    {
        dc = Dictionary::getInstance();
        mc = Multicast_3::getInstance();
    }

public:
    static Icast* getInstance()
    {
        static Icast instance_;
        return &instance_;
    }

    void operator=(const Icast&) = delete;
    Icast(Icast& other) = delete;

    Dictionary* dc;
    Multicast_3* mc;

    void init(bool print_structure = true);
    void update();
};

#endif