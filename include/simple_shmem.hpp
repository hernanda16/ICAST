#ifndef SIMPLE_SHMEM_HPP
#define SIMPLE_SHMEM_HPP

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <semaphore.h>
#include <fcntl.h>

class SimpleShmem
{
public:
    SimpleShmem(std::string key, size_t size, bool create = false)
    {
        key_ = ftok(key.c_str(), 1);
        size_ = size;
        create_ = create;
        key_str = key;

        if (create_)
        {
            shmid_ = shmget(key_, size_, IPC_CREAT | 0666);
        }
        else
        {
            shmid_ = shmget(key_, size_, 0666);
        }

        if (shmid_ < 0)
        {
            std::cerr << "shmget error" << std::endl;
            exit(1);
        }

        data_ = shmat(shmid_, NULL, 0);

        if (data_ == (void *)-1)
        {
            std::cerr << "shmat error" << std::endl;
            exit(1);
        }

        if (create_)
        {
            sem_ = sem_open(key.c_str(), O_CREAT, 0666, 1);
        }
        else
        {
            sem_ = sem_open(key.c_str(), 0);
        }

        if (sem_ == SEM_FAILED)
        {
            std::cerr << "sem_open error" << std::endl;
            exit(1);
        }
    }

    ~SimpleShmem()
    {
        shmdt(data_);
        shmctl(shmid_, IPC_RMID, NULL);
        sem_unlink(key_str.c_str());
        sem_close(sem_);
    }

    void lock()
    {
        sem_wait(sem_);
    }

    void unlock()
    {
        sem_post(sem_);
    }

    void *data()
    {
        return data_;
    }

private:
    key_t key_;
    int shmid_;
    size_t size_;
    sem_t *sem_;
    bool create_;
    std::string key_str;

public:
    void *data_;
};

#endif // SIMPLE_SHMEM_HPP