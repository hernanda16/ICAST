#include "multicast_v1.hpp"

bool Multicast_v1::readyToSend()
{
    // if (!initialized_)
    //     return false;

    // const uint64_t time_now = tick;

    // // printf("================================== %d\n", time_now);
    // if (time_now >= comm_time_next_tx_) {
    //     printf("MASUK KIRIM %d %d ============={%d]\n", time_now, comm_time_next_tx_, comm_period_);

    //     log_file << comm_time_next_tx_ << ",";

    //     updatePeers(tdma_my_ip_);

    //     // Calculate next transmission time
    //     comm_time_next_tx_ += comm_period_;

    //     if (friend_is_online)
    //         recvd_from_friend = 0;

    //     return true;
    // }

    // return false;
}

int Multicast_v1::send(std::vector<uint8_t> data, bool blocking)
{
}

int Multicast_v1::recv(std::vector<uint8_t> &data, bool blocking)
{
}
