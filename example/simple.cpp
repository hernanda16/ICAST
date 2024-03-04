#include "../include/icast.hpp"
#include "keyboard_input/keyboard_input.hpp"

void setDataToBeSend(Dictionary* dc_ptr);

int main()
{
    Icast* icast = Icast::getInstance();
    Dictionary* dc = Dictionary::getInstance();
    Multicast* mc = Multicast::getInstance();

    icast->init();

    while (true) {
        if (kbhit() > 0) {
            char key = std::cin.get();

            if (key == 's') {
                setDataToBeSend(dc);
            }
        }

        icast->update();

        float pose[3];
        size_t offset, size;
        dc->getOffsetSize(2, "pos", offset, size);
        std::memcpy(pose, dc->dictionary_data_.data() + offset, size);
        std::cout << "Pose: " << pose[0] << " " << pose[1] << " " << pose[2] << std::endl;
    }

    return 0;
}

void setDataToBeSend(Dictionary* dc_ptr)
{
    size_t offset, size;
    dc_ptr->getOffsetSize(0, "pos_offset", offset, size);
    std::cout << "Offset: " << offset << " Size: " << size << std::endl;

    struct data_tag {
        uint8_t agent = 2;
        float pose[3] = { 1.1, 2.2, 0.00 };
    } data_agent;

    std::memcpy(dc_ptr->dictionary_data_.data() + offset, &data_agent, size);

    dc_ptr->setResetUpdate(0, "pos_offset", false, true);
}
