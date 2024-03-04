#ifndef MULTICAST_ABSTRACT_HPP
#define MULTICAST_ABSTRACT_HPP

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "arpa/inet.h"
#include "net/if.h"
#include "netinet/in.h"
#include "stdint.h"
#include "string.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "sys/time.h"
#include "unistd.h"

class Multicast {
protected:
    bool initialized_ = false;

    int sock_;
    struct in_addr interface_addr_;
    struct sockaddr_in addr_;
    struct ip_mreq mreq_;

    uint64_t tick = 0;

public:
    Multicast() { }
    ~Multicast() { }

    /**
     * @brief Returns the current time in milliseconds.
     *
     * @return unsigned long int The current time in milliseconds.
     */
    uint64_t millis();

    /**
     * @brief Updates the internal tick counter.
    */
    void update_tick();

    /**
     * @brief Initializes the multicast object.
     *
     * @param ip The IP address of the multicast group.
     * @param port The port number of the multicast group.
     * @param interface The name of the network interface to use.
     * @param period_ms The period of the multicast communication in milliseconds.
     * @param loopback Whether to enable loopback mode (able recv myself).
     */
    void init(std::string ip, int port, std::string interface, uint16_t period_ms, uint8_t loopback = 0);

    /**
     * @brief Returns whether the multicast object is initialized.
     */
    bool initialized();

    /**
     * @brief Returns whether the multicast object is initialized.
     */
    virtual bool readyToSend() = 0;

    /**
     * @brief Sends data to the multicast group.
     *
     * @param data A vector of uint8_t containing the data to send.
     * @param blocking Whether to block until the data is sent.
     * @return int The number of bytes sent, or -1 if an error occurred.
     */
    virtual int send(std::vector<uint8_t> data, bool blocking = false) = 0;

    /**
     * @brief Receives data from the multicast group.
     *
     * @param data A reference to a vector of uint8_t to store the received data.
     * @param blocking Whether to block until data is received.
     * @return int The number of bytes received, or -1 if an error occurred.
     */
    virtual int recv(std::vector<uint8_t>& data, bool blocking = false) = 0;
};

#endif