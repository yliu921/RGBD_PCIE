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
#ifndef STREAM_INFRA_HPP
#define STREAM_INFRA_HPP
namespace tof = chronoptics::tof;
const int BUFFER_SIZE = 16;
const int LIMIT_SIZE= 16;

typedef struct Packet{
    /* data */
    float *z = nullptr;
    int frame_id;
}Packet;
std::queue<Packet> q;

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
#endif


