#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <string.h>
#include <queue>
#include <chronoptics/tof.hpp>
// #include <chronoptics/tof/Data.hpp>

#include "utils.hpp"
#include "data_structures.hpp"
#include "stream_infra.hpp"
namespace tof = chronoptics::tof;
pthread_mutex_t lock;
pthread_cond_t notempty;
pthread_cond_t notfull;

struct consumer_args {
    char *file_name; // arg[0]
    char direction;
    char *dev_name;
    uint64_t address;
    uint64_t size;
};

void *produce(void *data) {
    camera_stream cameras;
    if(cameras.get_camera() < 0) {
        PRINT_ERROR("Failed to get camera");
        return NULL;
    }
    if(cameras.start_camera_by_serial() < 0) {
        PRINT_ERROR("Failed to start camera");
        return NULL;
    }

    int i = 0;
    while(1) {
        // Capture
        Packet p;
        int size = cameras.get_frames(p);
        if(size <= 0) {
            PRINT_ERROR("Failed to get frames");
            continue;
        }
        std::cout<<"Capture " << i << " Middle Z: "<<p.z[153280]<<"\n";
        pthread_mutex_lock(&lock);
        while (q.size() == LIMIT_SIZE) {
            PRINT_INFO("Capture queue is full");
            pthread_cond_wait(&notfull, &lock);   
        }
        // Enqueue 
        p.frame_id = i++;
        q.push(p);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&notempty);         
    }
}
void *consume(void *args) {
    Packet t;
    struct consumer_args *cargs = (struct consumer_args *)args;
    std::cout << "Consume " << cargs->file_name << " " 
                            << cargs->direction << " " 
                            << cargs->dev_name << " " << cargs->address << "\n";

    while(1) {
        pthread_mutex_lock(&lock);
        while(q.size() == 0) {
            // PRINT_INFO("Capture queue is empty");
            pthread_cond_wait(&notempty, &lock);
        }
        t = q.front();
        q.pop();

        pthread_cond_signal(&notfull);
        pthread_mutex_unlock(&lock);
        std::cout<<"Consume "<< t.frame_id  <<" Middle Z: "<<t.z[153280]<<"\n";
        delete[] t.z;
        // printf("Consume%d\n", t.buffer[0]);
    }
}