#include "multicast.hpp"

void Multicast::findInterfaceIP(std::string interface, std::string& ip)
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

void Multicast::routineTDMA(const in_addr_t& addr)
{
    unsigned long int now = millis();

    // Delete peer if timestamp is too old
    for (auto it = tdma_peers_.begin(); it != tdma_peers_.end();) {
        if (now - it->ts > 2 * comm_period_) {
            it = tdma_peers_.erase(it);
        } else {
            ++it;
        }
    }

    // Update peer timestamp if exists
    bool if_exist = false;
    for (auto it = tdma_peers_.begin(); it != tdma_peers_.end(); ++it) {
        if (it->ip == addr) {
            it->ts = now;
            if_exist = true;
            break;
        }
    }

    // Add new peer to the end of the list if not exists,
    // also sort peers by IP address using std::sort
    if (!if_exist) {
        // Add new peer if not exists
        Peer peer;
        peer.ts = now;
        peer.ip = addr;
        tdma_peers_.push_back(peer);

        // Sort peers by IP address
        std::sort(tdma_peers_.begin(), tdma_peers_.end(), [](const Peer& a, const Peer& b) { return a.ip < b.ip; });
    }

    // Update my ID based on current peers
    for (unsigned int i = 0; i < tdma_peers_.size(); ++i) {
        if (tdma_peers_[i].ip == tdma_my_ip_) {
            tdma_my_id_ = i;
            break;
        }
    }
}

void Multicast::callbackTDMA(const in_addr_t& addr)
{
    comm_time_last_rx_ = millis();

    // Assign peer ID based on current peers
    for (unsigned int i = 0; i < tdma_peers_.size(); ++i) {
        if (tdma_peers_[i].ip == addr) {
            tdma_peer_id_ = i;
            break;
        }
    }

    // Calculate delta ID step to peer
    if (tdma_my_id_ > tdma_peer_id_) {
        tdma_peer_delta_id_ = tdma_my_id_ - tdma_peer_id_;
    } else {
        tdma_peer_delta_id_ = tdma_my_id_ - tdma_peer_id_ + tdma_peers_.size();
    }

    // Calculate next transmission time
    comm_time_next_tx_ = comm_time_last_rx_ + (comm_period_ * tdma_peer_delta_id_) / tdma_peers_.size();
}

unsigned long int Multicast::millis()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

//==============================================================================

void Multicast::init(std::string ip, int port, std::string interface)
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

    int loop = 1;
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

    unsigned long int now = millis();
    comm_time_next_tx_ = now;
    comm_time_last_rx_ = now;

    tdma_my_id_ = 0;
    tdma_my_ip_ = inet_addr(interface_ip.c_str());

    Peer peer;
    peer.ts = now;
    peer.ip = tdma_my_ip_;
    tdma_peers_.push_back(peer);

    initialized_ = true;
}

bool Multicast::ready()
{
    if (!initialized_)
        return false;

    if (millis() > comm_time_next_tx_) {
        routineTDMA(tdma_my_ip_);

        // Calculate next transmission time
        comm_time_next_tx_ += comm_period_;

        return true;
    }

    return false;
}

int Multicast::send(void* data, size_t len, bool blocking = false)
{
    if (!initialized_)
        return -1;

    return sendto(sock_, data, len, blocking ? 0 : MSG_DONTWAIT, (struct sockaddr*)&addr_, sizeof(addr_));
}

int Multicast::send(std::vector<uint8_t> data, bool blocking = false)
{
    if (!initialized_)
        return -1;

    return sendto(sock_, data.data(), data.size(), blocking ? 0 : MSG_DONTWAIT, (struct sockaddr*)&addr_, sizeof(addr_));
}

int Multicast::recv(void* data, size_t len, bool blocking = false)
{
    if (!initialized_)
        return -1;

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int ret = recvfrom(sock_, data, len, blocking ? 0 : MSG_DONTWAIT, (struct sockaddr*)&addr, &addrlen);
    if (ret <= 0) {
        memset(data, 0, len);
        return ret;
    }

    routineTDMA(addr.sin_addr.s_addr);
    callbackTDMA(addr.sin_addr.s_addr);

    return ret;
}

int Multicast::recv(std::vector<uint8_t>& data, bool blocking = false)
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

    routineTDMA(addr.sin_addr.s_addr);
    callbackTDMA(addr.sin_addr.s_addr);

    // std::cout << "Received " << ret << " bytes from " << inet_ntoa(addr.sin_addr) << std::endl;

    data = std::vector<uint8_t>(buf, buf + ret);
    return ret;
}