#include "../include/multicast3.hpp"

void Multicast_3::findInterfaceIP(std::string interface, std::string& ip)
{
    int fd;
    struct ifreq ifr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "socket error 1" << std::endl;
        return;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
        std::cerr << "ioctl error" << std::endl;
        return;
    }

    ip = inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);

    close(fd);
}

uint64_t Multicast_3::millis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Multicast_3::init(std::string ip, int port, std::string interface, uint16_t period_ms, uint8_t loopback)
{
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

    if ((sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        std::cerr << "socket error 2" << std::endl;
        exit(1);
    }

    if (bind(sock_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
        std::cerr << "bind error" << std::endl;
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt error 1" << std::endl;
        exit(1);
    }

    int loop = loopback;
    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loop, sizeof(loop)) < 0) {
        std::cerr << "setsockopt error 2" << std::endl;
        exit(1);
    }

    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_IF, (char*)&interface_addr_, sizeof(interface_addr_)) < 0) {
        std::cerr << "setsockopt error 3" << std::endl;
        exit(1);
    }

    if (setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq_, sizeof(mreq_)) < 0) {
        std::cerr << "setsockopt error 4" << std::endl;
        exit(1);
    }

    comm_period_ = period_ms;
    unsigned long int now = millis();
    comm_time_next_tx_ = now;
    comm_time_last_rx_ = now;

    tdma_my_ip_ = inet_addr(interface_ip.c_str());
    tdma_my_peer_index_ = 0;

    Peer peer;
    peer.ts = now;
    peer.ip = tdma_my_ip_;
    tdma_peers_.push_back(peer);

    initialized_ = true;
}

void Multicast_3::updatePeers()
{
    const uint32_t time_now = millis();

    for (uint8_t i = 0; i < tdma_peers_.size(); i++) {
        if (time_now - tdma_peers_[i].ts > 2 * comm_period_) {
            tdma_peers_.erase(tdma_peers_.begin() + i);
        }
    }

    for (uint8_t i = 0; i < tdma_peers_.size(); i++) {
        if (tdma_peers_[i].ip == tdma_my_ip_) {
            tdma_my_peer_index_ = i;
            break;
        }
    }

    tdma_peers_[tdma_my_peer_index_].prev_ts = tdma_peers_[tdma_my_peer_index_].ts;
    tdma_peers_[tdma_my_peer_index_].ts = time_now;
    tdma_peers_[tdma_my_peer_index_].period = tdma_peers_[tdma_my_peer_index_].ts - tdma_peers_[tdma_my_peer_index_].prev_ts;
}

void Multicast_3::addToPeer(uint32_t ip)
{
    const uint32_t time_now = millis();
    const uint8_t short_ip = ip & 0xFF;

    uint8_t is_exist = 0x00000000;
    for (uint8_t i = 0; i < tdma_peers_.size(); i++) {
        if (tdma_peers_[i].ip == short_ip) {
            is_exist = 0x00000001;
        }
    }

    if (!is_exist) {
        Peer peer;
        peer.ts = time_now;
        peer.prev_ts = time_now;
        peer.ip = short_ip;
        peer.period = 0;
        peer.distance = 0;
        tdma_peers_.push_back(peer);

        // sort peers by ip
        std::sort(tdma_peers_.begin(), tdma_peers_.end(), [](const Peer& a, const Peer& b) {
            return a.ip < b.ip;
        });

        updatePeers();

        return;
    }

    uint8_t recv_from_peer_index = 0;
    for (uint8_t i = 0; i < tdma_peers_.size(); i++) {
        if (tdma_peers_[i].ip == tdma_my_ip_) {
            recv_from_peer_index = i;
            break;
        }
    }

    tdma_peers_[recv_from_peer_index].prev_ts = tdma_peers_[recv_from_peer_index].ts;
    tdma_peers_[recv_from_peer_index].ts = time_now;
    tdma_peers_[recv_from_peer_index].period = tdma_peers_[recv_from_peer_index].ts - tdma_peers_[tdma_my_peer_index_].prev_ts;

    if (tdma_my_peer_index_ > tdma_peers_[recv_from_peer_index].ip)
        tdma_peers_[recv_from_peer_index].distance = recv_from_peer_index + tdma_peers_.size() - tdma_my_peer_index_;
    else
        tdma_peers_[recv_from_peer_index].distance = recv_from_peer_index - tdma_my_peer_index_;

    // check last distance sequence from pair
    if (tdma_peers_[recv_from_peer_index].distance == tdma_peers_.size() - 1) {
        if ((float)(tdma_peers_[recv_from_peer_index].period / comm_period_) < 1.1)
            comm_period_ -= 2;
        else
            comm_period_ += 2;

        if (comm_period_ < 5)
            comm_period_ = 5;
        if (comm_period_ > 1000)
            comm_period_ = 1000;

        comm_time_next_tx_ = comm_time_last_rx_ + comm_period_ / tdma_peers_.size();
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

    if (millis() > comm_time_next_tx_) {
        updatePeers();

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

    return sendto(sock_, data.data(), data.size(), blocking ? 0 : MSG_DONTWAIT, (struct sockaddr*)&addr_, sizeof(addr_));
}

int Multicast_3::recv(std::vector<uint8_t>& data, bool blocking)
{
    if (!initialized_)
        return -1;

    char buf[1024];

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int ret = recvfrom(sock_, buf, sizeof(buf), blocking ? 0 : MSG_DONTWAIT, (struct sockaddr*)&addr, &addrlen);
    if (ret <= 0) {
        data.clear();
        return ret;
    }

    addToPeer(addr.sin_addr.s_addr);

    // std::cout << "Received " << ret << " bytes from " << inet_ntoa(addr.sin_addr) << std::endl;

    data = std::vector<uint8_t>(buf, buf + ret);
    return ret;
}