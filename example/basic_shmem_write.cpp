#include "simple_shmem.hpp"
#include "signal.h"
#include "stdint.h"

typedef struct
{
    int16_t pose_x;
    int16_t pose_y;
    float pose_theta;
} data_t;

SimpleShmem shared_memory("basic_example", 8, true);

void signal_handler(int signum);

int main()
{
    signal(SIGINT, signal_handler);

    data_t *data = (data_t *)shared_memory.data_;

    shared_memory.lock();
    data->pose_x = 123;
    data->pose_y = 321;
    data->pose_theta = 10.2;
    shared_memory.unlock();

    while (1)
    {
        shared_memory.lock();
        data->pose_x++;
        printf("WRITE DATA: %d %d %.2f\n", data->pose_x, data->pose_y, data->pose_theta);
        shared_memory.unlock();

        usleep(1000000);
    }

    return 0;
}

void signal_handler(int signum)
{
    printf("Interrupt signal (%d) received.\n", signum);
    printf("Destroying shared memory...\n");
    shared_memory.~SimpleShmem();

    exit(signum);
}