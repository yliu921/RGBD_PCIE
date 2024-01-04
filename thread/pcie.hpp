#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/timeb.h>

#ifndef PCIE_HPP
#define PCIE_HPP
#define  DMA_MAX_SIZE   0x10000000UL
char USAGE [] = 
    "Usage: \n"
    "\n"
    "  write device (host-to-device):\n"
    "    %s <file_name> to <device_name> <address_in_the_device> <size>\n"
    "  example:\n"
    "    %s data.bin to /dev/xdma0_h2c_0 0x100000 0x10000\n"
    "\n"
    "  read device (device-to-host):\n"
    "    %s <file_name> from <device_name> <address_in_the_device> <size>\n"
    "  example:\n"
    "    %s data.bin from /dev/xdma0_c2h_0 0x100000 0x10000\n" ;


// function : dev_read
// description : read data from device to local memory (buffer), (i.e. device-to-host)
// parameter :
//       dev_fd : device instance
//       addr   : source address in the device
//       buffer : buffer base pointer
//       size   : data size
// return:
//       int : 0=success,  -1=failed
int dev_read (int dev_fd, uint64_t addr, void *buffer, uint64_t size) {
    if ( addr != lseek(dev_fd, addr, SEEK_SET) )                                 // seek
        return -1;                                                               // seek failed
    if ( size != read(dev_fd, buffer, size) )                                    // read device to buffer
        return -1;                                                               // read failed
    return 0;
}


// function : dev_write
// description : write data from local memory (buffer) to device, (i.e. host-to-device)
// parameter :
//       dev_fd : device instance
//       addr   : target address in the device
//       buffer : buffer base pointer
//       size   : data size
// return:
//       int : 0=success,  -1=failed
int dev_write (int dev_fd, uint64_t addr, void *buffer, uint64_t size) {
    if ( addr != lseek(dev_fd, addr, SEEK_SET) )                                 // seek
        return -1;                                                               // seek failed
    if ( size != write(dev_fd, buffer, size) )                                   // write device from buffer
        return -1;                                                               // write failed
    return 0;
}

// function : get_millisecond
// description : get time in millisecond
static uint64_t get_millisecond () {
    struct timeb tb;
    ftime(&tb);
    return (uint64_t)tb.millitm + (uint64_t)tb.time * 1000UL;
    // tb.time is the number of seconds since 00:00:00 January 1, 1970 UTC time;
    // tb.millitm is the number of milliseconds in a second
}
#endif






