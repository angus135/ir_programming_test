#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
// Asked Generative AI for a good library for mutexes (would use something like a freeRTOS mutex here)
// but for the purposes of this it demonstrates what is needed. Additionally, you could probably just
// use the inbuilt freeRTOS queues rather than this custom queue as it handles all of this stuff.
#include <pthread.h> 
#include "macros.h"

typedef struct { 
    uint8_t* data;
    uint16_t front;
    uint16_t rear;
    uint16_t size;
    uint16_t array_length;
    pthread_mutex_t mutex;
} Queue;

Queue* initialise_queue(uint16_t max_queue_size);

void delete_queue(Queue* queue);

Status dequeue(Queue* queue, uint8_t* data);

Status enqueue(Queue* queue, uint8_t data);

// Return 0 if not full, 1 if full and 2 if there's an error
uint8_t is_queue_full(Queue* queue);

#endif