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
#include "../utils.hpp"
#include "../data_structures.hpp"
namespace tof = chronoptics::tof;

const int BUFFER_SIZE = 16;
typedef struct Packet{
    /* data */
    float *z = nullptr;
    int frame_id;
}Packet;
std::queue<Packet> q;
const int LIMIT_SIZE= 16;
pthread_mutex_t lock;
pthread_cond_t notempty;
pthread_cond_t notfull;
class camera_stream{
public :
    std::vector<tof::DiscoveredKea> cameras;
    tof::KeaCamera cam;
    float *z=nullptr;
    int get_camera() {
        cameras = tof::discover_kea_cameras();
        if (cameras.empty()) {
            std::cout << "No camera found\n";
            return -1;
        }
        return 0;
    }
    int start_camera_by_serial() {
        cam = tof::KeaCamera(cameras.front().serial());
        PRINT_INFO2("Streaming from camera ", cam.get_serial());
        int ret = tof::select_streams(cam, {tof::FrameType::Z, 
                                  tof::FrameType::BGR_PROJECTED});
        if(ret != 2) {
            PRINT_INFO2("Failed to select streams:", ret);
        }
        cam.start();
        return 0;
    }
    int stop_camera() {
        cam.stop();
        return 0;
    }
    int get_frames(Packet &p) {
        std::vector<tof::Data> frames;
        frames = cam.get_frames();
        for(auto &frame : frames) {
            if(frame.frame_type() == tof::FrameType::Z) {
                // z = tof::cast_data_safe<float>(frame);
                z = cast_data_safe<float>(frame);
                p.z = new float[frame.size()];
                memcpy(p.z, z, frame.size());
                return frame.size();
            }
        }
        return 0;
    }

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
        pthread_mutex_lock(&lock);
        while (q.size() == LIMIT_SIZE) {
            PRINT_INFO("Capture queue is full");
            pthread_cond_wait(&notfull, &lock);   
        }
        // Enqueue 
        std::cout<<"Capture " << i << " Middle Z: "<<p.z[153280]<<"\n";
        p.frame_id = i++;
        q.push(p);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&notempty);         
    }
}
void *consume(void *data) {
    Packet t;
    while(1) {
        pthread_mutex_lock(&lock);
        while(q.size() == 0) {
            // PRINT_INFO("Capture queue is empty");
            pthread_cond_wait(&notempty, &lock);
        }
        t = q.front();
        q.pop();
        std::cout<<"Consume "<< t.frame_id  <<" Middle Z: "<<t.z[153280]<<"\n";
                delete[] t.z;
        pthread_cond_signal(&notfull);
        pthread_mutex_unlock(&lock);
        // printf("Consume%d\n", t.buffer[0]);
        

    }
}
int main(int argc, char const *argv[])  {
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&notempty, NULL);
    pthread_cond_init(&notfull, NULL);
    pthread_t producer, consumer;
    pthread_create(&producer, NULL, produce, NULL);
    pthread_create(&consumer, NULL, consume, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&notempty);
    pthread_cond_destroy(&notfull);
}
