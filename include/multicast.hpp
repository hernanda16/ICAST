/**
 * @file multicast.hpp
 * @author Pandu Surya Tantra (pandustantra@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IRISDB_SERVER__MULTICAST_HPP
#define IRISDB_SERVER__MULTICAST_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#include "arpa/inet.h"
#include "net/if.h"
#include "netinet/in.h"
#include "string.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "unistd.h"

typedef struct
{
    unsigned long int ts;
    unsigned int ip;
} Peer;

class Multicast {
private:
    bool initialized_ = false;

    int sock_;
    struct in_addr interface_addr_;
    struct sockaddr_in addr_;
    struct ip_mreq mreq_;

    unsigned long int comm_period_ = 1000;
    unsigned long int comm_time_next_tx_ = 0;
    unsigned long int comm_time_last_rx_ = 0;

    unsigned int tdma_my_ip_;
    unsigned int tdma_my_id_;
    std::vector<Peer> tdma_peers_;
    unsigned int tdma_peer_id_;
    unsigned int tdma_peer_delta_id_;

    /**
     * @brief Finds the IP address of a given network interface.
     *
     * @param interface The name of the network interface.
     * @param ip The IP address of the network interface.
     */
    void findInterfaceIP(std::string interface, std::string& ip);
    void routineTDMA(const in_addr_t& addr);
    void callbackTDMA(const in_addr_t& addr);

    /**
     * @brief Returns the current time in milliseconds.
     *
     * @return unsigned long int The current time in milliseconds.
     */
    unsigned long int millis();

public:
    Multicast()
    {
    }

    /**
     * @brief Initializes the multicast object.
     *
     * @param ip The IP address of the multicast group.
     * @param port The port number of the multicast group.
     * @param interface The name of the network interface to use.
     */
    void init(std::string ip, int port, std::string interface);
    bool ready();
    bool initialized();

    /**
     * @brief Sends data to the multicast group.
     *
     * @param data A pointer to the data to send.
     * @param len The length of the data to send.
     * @param blocking Whether to block until the data is sent.
     * @return int The number of bytes sent, or -1 if an error occurred.
     */
    int send(void* data, size_t len, bool blocking = false);

    /**
     * @brief Sends data to the multicast group.
     *
     * @param data A vector of uint8_t containing the data to send.
     * @param blocking Whether to block until the data is sent.
     * @return int The number of bytes sent, or -1 if an error occurred.
     */
    int send(std::vector<uint8_t> data, bool blocking = false);

    /**
     * @brief Receives data from the multicast group.
     *
     * @param data A pointer to the buffer to store the received data.
     * @param len The length of the buffer.
     * @param blocking Whether to block until data is received.
     * @return int The number of bytes received, or -1 if an error occurred.
     */
    int recv(void* data, size_t len, bool blocking = false);

    /**
     * @brief Receives data from the multicast group.
     *
     * @param data A reference to a vector of uint8_t to store the received data.
     * @param blocking Whether to block until data is received.
     * @return int The number of bytes received, or -1 if an error occurred.
     */
    int recv(std::vector<uint8_t>& data, bool blocking = false);
};

#endif // IRISDB_SERVER__MULTICAST_HPP