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
#include "pcie.hpp"
#include "utils.hpp"
#include "data_structures.hpp"
#include "stream_infra.hpp"
#include "pthread_infra.hpp"

namespace tof = chronoptics::tof;

int main(int argc, char *argv[])  {
    if(argc < 4) {
        PRINT_ERROR("Not enough arguments");
        return -1;
    }
    struct consumer_args cargs;// = new struct consumer_args; // consumer args
    // cargs.file_name = argv[1];
    // cargs.direction = argv[2][0];
    cargs.dev_name = argv[1];
    if(0 == parse_uint(argv[2], &cargs.address)) {
        PRINT_ERROR("Failed to parse address");
        return -1;
    }
    if(0 == parse_uint(argv[3], &cargs.size)) {
        PRINT_ERROR("Failed to parse size");
        return -1;
    }
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&notempty, NULL);
    pthread_cond_init(&notfull, NULL);
    pthread_t producer, consumer;
    pthread_create(&producer, NULL, produce, NULL);
    pthread_create(&consumer, NULL, consume, (void*)&cargs);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&notempty);
    pthread_cond_destroy(&notfull);
    printf("Main thread exiting\n");
    return 0;
}
