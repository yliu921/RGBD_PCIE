#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/timeb.h>

#include <chronoptics/tof.hpp>
#include "data_structures.hpp"
#include "pcie.hpp"
#include "utils.hpp"


namespace tof = chronoptics::tof;

int main(int argc, char *argv[]) {
    // PCIE inputs
    int   ret = -1;
    uint64_t millisecond;
    char usage_string [1024];
    char *dev_name, *file_name;
    char direction;
    uint64_t address;
    int   dev_fd = -1;
    FILE *file_p = NULL;
    void *buffer = NULL;
    
    sprintf(usage_string, USAGE, argv[0], argv[0], argv[0], argv[0] );
    if (argc < 5) { // not enough argument
        puts(usage_string);
        return -1;
    }
        
    file_name = argv[1];
    direction = argv[2][0];
    dev_name  = argv[3];
    if(0 == parse_uint(argv[4], &address)) { // parse the address in the device
        puts(usage_string);
        return -1;
    }
    
    uint64_t size[2] = {0,0};


    // get the camera device
    std::vector<tof::DiscoveredKea> cameras = tof::discover_kea_cameras();
    if (cameras.empty()) {
        std::cout << "No camera found\n";
        return -1;
    }

    tof::KeaCamera cam(cameras.front().serial());
    //tof::KeaCamera cam("202005a");
    std::cout << "Streaming from camera " << cam.get_serial() << "\n";
    tof::select_streams(cam, {tof::FrameType::Z, tof::FrameType::BGR_PROJECTED});
    cam.start();
    auto frames = cam.get_frames();
    float *z = nullptr;
    BGR *bgr = nullptr;
    for (auto &frame : frames) {
        switch (frame.frame_type()) {
            case tof::FrameType::Z: {
                z = cast_data_safe<float>(frame);
                std::cout << "Middle Z pixel value: " << z[153280] << "\n";
                std::cout << "frame siez: "<< frame.size() // row*col*4 
                          << " row: "<< frame.rows() 
                          << " cols: "<< frame.cols()<< "\n"; 
                size[0] = frame.size();
                std::cout << "address:"<<z<<"\n";
                break;
            }
            case tof::FrameType::BGR_PROJECTED: {
                bgr = cast_data_safe<BGR>(frame);
                std::cout << "Middle blue pixel value: " << (int)bgr[153280].b << "\n";
                std::cout << "frame siez: "<< frame.size() // row*col*4 
                      << " row: "<< frame.rows() 
                      << " cols: "<< frame.cols()<< "\n"; 
                std::cout << "address:"<<bgr<<"\n";
                size[1] = frame.size();
                break;
            }
            default:
                std::cout << "Unexpected frame type\n";
                break;
        }
    }
    // return -1; 
    // print information:
    if(direction == 't') {  // to (write device, host-to-device)
        printf("from : %s\n" , file_name);
        printf("to   : %s   addr=0x%lx\n" , dev_name, address);
        printf("size : 0x%lx\n\n" , size[0]);
    } else if (direction == 'f') { // from (read device, device-to-host)
        printf("from : %s   addr=0x%lx\n" , dev_name, address);
        printf("to   : %s\n" , file_name);
        printf("size : 0x%lx\n\n" , size[0]);
    } else {
        puts(usage_string);
        return -1;
    }
    if ((size[0] + size[1]) > DMA_MAX_SIZE  ||  (size[0] == 0) || (size[1] == 0)) {
        printf("*** ERROR: DMA size must larger than 0 and NOT be larger than %lu\n", DMA_MAX_SIZE);
        return -1;
    }
    
    dev_fd = open(dev_name, O_RDWR);                                         // open device
    if (dev_fd < 0) {
        printf("*** ERROR: failed to open device %s\n", dev_name);
        return -1;
    } 

    uint8_t *data = new uint8_t[size[0]]; // (uint8_t *)malloc(sizeof(uint8_t)*size); 
    buffer = malloc(size[0]);        // allocate local memory (buffer)
    if (buffer == NULL) {
        printf("*** ERROR: failed to allocate buffer\n");
        //goto close_and_clear;
        return -1;
    }
 #ifdef DEBUG
    uint8_t *b = (uint8_t *)z; 
    for(int ii = 0; ii < size[0]; ++ii) {
        b[ii] = ii;
        printf("0x%02x ", b[ii]);
    }
    std::cout << "\n";
#endif

    PRINT_INFO("Start streaming..");

    millisecond = get_millisecond(); // start time
    //if (direction == 't') {
        // if(size[0] != fread(buffer, 1, size[0], file_p) ) {                      // local file -> local buffer
        //     printf("*** ERROR: failed to read %s\n", file_name);
        //     goto close_and_clear;
        // }
    if(dev_write(dev_fd, address, (void *)z, size[0])) {// local buffer -> device
        printf("*** ERROR: failed to write %s\n", dev_name);
        goto close_and_clear;
    }
    //} 
    // else {
    //     if(dev_read(dev_fd, address, buffer, size[0]) ) {                     // device -> local buffer
    //         printf("*** ERROR: failed to read %s\n", dev_name);
    //         goto close_and_clear;
    //     }
    //     if(size != fwrite(buffer, 1, size[0], file_p) ) {                     // local buffer -> local file
    //         printf("*** ERROR: failed to write %s\n", file_name);
    //         goto close_and_clear;
    //     }
    // }
    
    millisecond = get_millisecond() - millisecond;      // get time consumption
    millisecond = (millisecond > 0) ? millisecond : 1;  // avoid divide-by-zero
    
    PRINT_INFO("Finish streaming..");
    PRINT_INFO2(millisecond, " ms"); 
    PRINT_INFO2((double)size[0]/millisecond/1000, " MBps");
    ret = 0;
    
close_and_clear:
    
    if (buffer != NULL)
        free(buffer);
    
    if (dev_fd >= 0)
        close(dev_fd);
    
    if (file_p != NULL)
        fclose(file_p);
    delete data; 
    return ret; 
}