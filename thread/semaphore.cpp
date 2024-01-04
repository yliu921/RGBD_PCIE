#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
typedef class sbuf_t{
public:
    int *buf;
    int n;
    int front, rear;
    sem_t mutex, slots, items;
    void init(int nn){
        buf = new int[nn];
        n = nn;
        front = rear = 0;
        sem_init(&mutex, 0, 1);
        sem_init(&slots, 0, n);
        sem_init(&items, 0, 0);
    }
    ~sbuf_t(){
        delete[] buf;
    }
    
    /**
     * Inserts an item into the buffer.
     * @param item the item to be inserted
     * @throws None
     */
    void insert(int item){
        sem_wait(&slots);
        sem_wait(&mutex);

        buf[rear] = item;
        rear = (rear + 1) % n;
        printf("Rear:%d\n", rear);
        sem_post(&mutex);
        sem_post(&items);
    }   
    int remove(){
        sem_wait(&items);
        sem_wait(&mutex);
        int item = buf[front];
        front = (front + 1) % n;
        sem_post(&mutex);
        sem_post(&slots);
        return item;
    }
};

sbuf_t sbuf;
void *thread_capture(void *argp) {
    int ret = pthread_detach(pthread_self());
    int ii = 0;
    while(1) {
        //int item=0;
        //scanf("%d", &item);
        sbuf.insert(ii);
        printf("Capture:%d\n", ii);
        sleep(1);
        // if(item == 0) {
        //     break;
        // }
        ii++;
    }
}
void *thread_forward(void *argp) {
    int ret = pthread_detach(pthread_self());
    while(1) {
        int item = sbuf.remove();
        sleep(2);
        printf("Forward:%d\n", item);
    }
}
int main(int argc, char *argv[]){
    printf("Hello, World!\n");
    sbuf.init(100);
    pthread_t t1, t2;
    sbuf.init(2);
    pthread_create(&t1, NULL, thread_capture, NULL);
    pthread_create(&t2, NULL, thread_forward, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}