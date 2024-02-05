#include "../include/icast.hpp"
#include "../utils/keyboard_input/keyboard_input.hpp"

void setDataToBeSend(Dictionary* dc_ptr);

int main()
{
    Dictionary dc;
    Multicast mc;

    mc.init("224.16.32.80", 8888, "wlp3s0");
    if (!mc.initialized()) {
        std::cout << "Multicast not ready" << std::endl;
        return 0;
    }

    dc.init(0, "../dictionary.json");
    dc.structureDisplay();

    while (true) {
        if (kbhit() > 0) {
            char key = std::cin.get();
            std::cout << key << std::endl;

            if (key == 's') {
                setDataToBeSend(&dc);
            }
        }

        if (mc.ready()) {
            std::vector<uint8_t> selected_data;
            dc.packetProcessTransmit(selected_data);
            if (selected_data.size() > 0)
                mc.send(selected_data);
        }
    }

    return 0;
}

void setDataToBeSend(Dictionary* dc_ptr)
{
    size_t offset, size;
    dc_ptr->getOffsetSize(0, "pos_offset", offset, size);
    std::cout << "Offset: " << offset << " Size: " << size << std::endl;

    struct data_tag {
        uint8_t agent = 1;
        float pose[3] = { 1.1, 2.2, 3.3 };
    } data_agent;

    std::memcpy(dc_ptr->dictionary_data_.data() + offset, &data_agent, size);

    dc_ptr->setResetUpdate(0, "pos_offset", false, true);
}
