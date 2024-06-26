#ifndef ICAST_HPP
#define ICAST_HPP

#include "dictionary.hpp"
#include "multicast3.hpp"
#include <sched.h>

class Icast
{
private:
    Icast()
    {
        dc = Dictionary::getInstance();
        mc = Multicast_3::getInstance();
        mc_loopback = 0;
    }

    uint8_t mc_loopback;

    std::string dict_path;

public:
    static Icast *getInstance()
    {
        static Icast instance_;
        return &instance_;
    }

    void operator=(const Icast &) = delete;
    Icast(Icast &other) = delete;

    Dictionary *dc;
    Multicast_3 *mc;

    void init(std::string config_path = "", bool print_structure = true);
    void update();
    void update_send();
    void update_receive();
    void setMcLoopback(uint8_t mc_loopback);
};

#endif