#include "icast.hpp"

void set_data_to_be_send();
void get_some_data_from_icast_bus();

int main()
{
    Icast *icast = Icast::getInstance();

    icast->init();

    if (!icast->mc->initialized())
    {
        return -1;
    }

    while (1)
    {
        set_data_to_be_send();

        icast->update();

        get_some_data_from_icast_bus();

        usleep(1000000);
    }

    return 0;
}

void set_data_to_be_send()
{
    static Icast *icast = Icast::getInstance();

    static pos_t pos;

    static uint8_t first_init = 1;

    if (first_init == 1)
    {
        pos.x = 123;
        pos.y = 321;
        pos.theta = 10.2;

        first_init = 0;
    }

    icast->dc->setDataToBeSent("pos", (void *)&pos);

    pos.x++;
}

void get_some_data_from_icast_bus()
{
    static Icast *icast = Icast::getInstance();

    pos_t my_pos;

    memcpy(&my_pos, &icast->dc->data_bus.agent1.pos.x, sizeof(pos_t));

    my_pos.x = icast->dc->data_bus.agent1.pos.x;

    printf("MY POS: %d %d %.2f\n", my_pos.x, my_pos.y, my_pos.theta);
}