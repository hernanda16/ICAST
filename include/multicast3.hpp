#ifndef MULTICAST3_HPP
#define MULTICAST3_HPP

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <fstream>

#include "arpa/inet.h"
#include "net/if.h"
#include "netinet/in.h"
#include "stdint.h"
#include "string.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "unistd.h"
#include <sys/time.h>

<<<<<<< HEAD
class Multicast_3 {
=======
#define USE_CHRONO_TIMER

class Multicast_3
{
>>>>>>> d28ccdf94b11173f904be0829db7f413edcc32b4
private:
    Multicast_3() { }

    bool initialized_ = false;

    int sock_;
    struct in_addr interface_addr_;
    struct sockaddr_in addr_;
    struct ip_mreq mreq_;

    unsigned long int comm_period_ = 1000;
    unsigned long int comm_time_next_tx_ = 0;
    unsigned long int comm_time_last_rx_ = 0;
    unsigned long int comm_period_min_ = 10;
    unsigned long int comm_period_max_ = 1000;
    float comm_period_threshold_ = 0.1;
    const uint16_t dead_threshold_ms = 5000;

    uint8_t tdma_my_ip_;
    uint8_t tdma_my_peer_index_;

    std::vector<uint8_t> peer_ip;
    std::vector<uint64_t> peer_ts;
    std::vector<uint64_t> peer_prev_ts;

    // Percobaan
    uint8_t friend_is_online = 0;
    uint8_t recvd_from_friend = 0;

    uint64_t wall_time_start_;

    /**
     * @brief Finds the IP address of a given network interface.
     *
     * @param interface The name of the network interface.
     * @param ip The IP address of the network interface.
     */
<<<<<<< HEAD
    void findInterfaceIP(std::string interface, std::string& ip);
=======
    void findInterfaceIP(std::string interface, std::string &ip);

    std::ofstream log_file;
>>>>>>> d28ccdf94b11173f904be0829db7f413edcc32b4

public:
    static Multicast_3* getInstance()
    {
        static Multicast_3 instance_;
        return &instance_;
    }

    void operator=(const Multicast_3&) = delete;
    Multicast_3(Multicast_3& other) = delete;

    std::ofstream log_file;

    /**
     * @brief Returns the current time in milliseconds.
     *
     * @return unsigned long int The current time in milliseconds.
     */
    uint64_t millis();
    uint64_t tick;

    /**
     * @brief Returns the current time in milliseconds.
     *
     * @return unsigned long int The current time in milliseconds.
     */
    uint64_t millis();

    uint64_t tick = 0;

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
     * @brief Updates the TDMA peers sequence and period based on the current time.
     */
    void updatePeers(uint8_t ip);

    /**
     * @brief Add a peer to the TDMA sequence if it is not already present.
     */
    void addToPeer(uint32_t ip);

    /**
     * @brief Returns whether the multicast object is initialized.
     */
    bool initialized();

    /**
     * @brief Returns whether the multicast object is initialized.
     */
    bool readyToSend();

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
     * @param data A reference to a vector of uint8_t to store the received data.
     * @param blocking Whether to block until data is received.
     * @return int The number of bytes received, or -1 if an error occurred.
     */
    int recv(std::vector<uint8_t>& data, bool blocking = false);
};

#endif