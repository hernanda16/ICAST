#include "../include/multicast3.hpp"

void Multicast_3::findInterfaceIP(std::string interface, std::string &ip)
{
    int fd;
    struct ifreq ifr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "socket error 1" << std::endl;
        return;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    {
        std::cerr << "ioctl error" << std::endl;
        return;
    }

    ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

    close(fd);
}

uint64_t Multicast_3::millis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - wall_time_start_;
}

void Multicast_3::init(std::string ip, int port, std::string interface, uint16_t period_ms, uint8_t loopback)
{
    wall_time_start_ = millis() & 0xFFFFFFFF;

    std::string interface_ip;
    findInterfaceIP(interface, interface_ip);

    memset(&interface_addr_, 0, sizeof(interface_addr_));
    interface_addr_.s_addr = inet_addr(interface_ip.c_str());

    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);

    memset(&mreq_, 0, sizeof(mreq_));
    mreq_.imr_multiaddr.s_addr = inet_addr(ip.c_str());
    mreq_.imr_interface.s_addr = inet_addr(interface_ip.c_str());

    if ((sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        std::cerr << "socket error 2" << std::endl;
        exit(1);
    }

    if (bind(sock_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
    {
        std::cerr << "bind error" << std::endl;
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt error 1" << std::endl;
        exit(1);
    }

    int loop = loopback;
    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop, sizeof(loop)) < 0)
    {
        std::cerr << "setsockopt error 2" << std::endl;
        exit(1);
    }

    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_IF, (char *)&interface_addr_, sizeof(interface_addr_)) < 0)
    {
        std::cerr << "setsockopt error 3" << std::endl;
        exit(1);
    }

    if (setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq_, sizeof(mreq_)) < 0)
    {
        std::cerr << "setsockopt error 4" << std::endl;
        exit(1);
    }

    comm_period_ = period_ms;
    unsigned long int now = millis();
    comm_time_next_tx_ = now;
    comm_time_last_rx_ = now;

    const uint32_t ip_buffer = inet_addr(interface_ip.c_str());
    tdma_my_peer_index_ = 0;
    tdma_my_ip_ = (ip_buffer & ~(uint32_t)0xFF) >> 24;

    addToPeer(ip_buffer);

    initialized_ = true;
}

void Multicast_3::updatePeers(uint8_t ip)
{
    const uint64_t time_now = millis() & 0xFFFFFFFF;

    // update ts of ip in peer list
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        if (peer_ip[i] == ip)
        {
            peer_prev_ts[i] = peer_ts[i];
            peer_ts[i] = time_now;
            break;
        }
    }

    // delete peers that have not been heard from in a while
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        if (time_now - peer_ts[i] > dead_threshold_ms)
        {
            peer_ip.erase(peer_ip.begin() + i);
            peer_ts.erase(peer_ts.begin() + i);
            peer_prev_ts.erase(peer_prev_ts.begin() + i);
            i--;
        }
    }

    // Find my id
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        if (peer_ip[i] == tdma_my_ip_)
        {
            tdma_my_peer_index_ = i;
            break;
        }
    }
}

void Multicast_3::addToPeer(uint32_t ip)
{
    const uint64_t time_now = millis() & 0xFFFFFFFF;
    const uint8_t short_ip = (ip & ~(uint32_t)0xFF) >> 24;

    // check if ip is already in peer list
    uint8_t is_peer_exist = 0;
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        if (peer_ip[i] == short_ip)
        {
            is_peer_exist = 1;
            break;
        }
    }

    if (!is_peer_exist)
    {
        peer_ip.push_back(short_ip);
        peer_ts.push_back(time_now);
        peer_prev_ts.push_back(time_now);

        // sort peer_ip, peer_ts, peer_prev_ts based on peer_ip
        for (size_t i = 0; i < peer_ip.size(); i++)
        {
            for (size_t j = i + 1; j < peer_ip.size(); j++)
            {
                if (peer_ip[i] > peer_ip[j])
                {
                    std::swap(peer_ip[i], peer_ip[j]);
                    std::swap(peer_ts[i], peer_ts[j]);
                    std::swap(peer_prev_ts[i], peer_prev_ts[j]);
                }
            }
        }

        updatePeers(short_ip);

        return;
    }

    updatePeers(short_ip);

    // print peers
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        printf("peer_ip: %d || peer_ts: %d || peer_prev_ts: %d\n", peer_ip[i], peer_ts[i], peer_prev_ts[i]);
    }

    // find recv_from index
    uint8_t recv_from_index = 0;
    for (size_t i = 0; i < peer_ip.size(); i++)
    {
        if (peer_ip[i] == short_ip)
        {
            recv_from_index = i;
            break;
        }
    }

    // Calculate hop distance
    uint8_t hop_distance = 0;
    if (tdma_my_peer_index_ > recv_from_index)
    {
        hop_distance = recv_from_index + peer_ip.size() - tdma_my_peer_index_;
    }
    else
    {
        hop_distance = recv_from_index - tdma_my_peer_index_;
    }

    // Calculate period
    if (hop_distance == peer_ip.size() - 1)
    {
        if ((float)(peer_ts[recv_from_index] - peer_prev_ts[recv_from_index]) / comm_period_ < 1.1)
            comm_period_ -= 2;
        else
            comm_period_ += 2;

        if (comm_period_ < comm_period_min_)
            comm_period_ = comm_period_min_;
        else if (comm_period_ > comm_period_max_)
            comm_period_ = comm_period_max_;

        printf("comm_period_: %d || REAL %d\n", comm_period_, peer_ts[recv_from_index] - peer_prev_ts[recv_from_index]);

        comm_time_next_tx_ = peer_ts[recv_from_index] + comm_period_ / peer_ip.size();
    }
}

bool Multicast_3::initialized()
{
    return initialized_ == true ? true : false;
}

bool Multicast_3::readyToSend()
{
    if (!initialized_)
        return false;

    if (millis() > comm_time_next_tx_)
    {
        updatePeers(tdma_my_ip_);

        // Calculate next transmission time
        comm_time_next_tx_ += comm_period_;

        return true;
    }

    return false;
}

int Multicast_3::send(std::vector<uint8_t> data, bool blocking)
{
    if (!initialized_)
        return -1;

    return sendto(sock_, data.data(), data.size(), blocking ? 0 : MSG_DONTWAIT, (struct sockaddr *)&addr_, sizeof(addr_));
}

int Multicast_3::recv(std::vector<uint8_t> &data, bool blocking)
{
    if (!initialized_)
        return -1;

    char buf[1024];

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int ret = recvfrom(sock_, buf, sizeof(buf), blocking ? 0 : MSG_DONTWAIT, (struct sockaddr *)&addr, &addrlen);
    if (ret <= 0)
    {
        data.clear();
        return ret;
    }

    addToPeer(addr.sin_addr.s_addr);

    data = std::vector<uint8_t>(buf, buf + ret);
    return ret;
}