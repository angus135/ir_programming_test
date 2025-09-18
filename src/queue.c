#include "queue.h"
#include <stdlib.h>
#include "macros.h"

Queue* initialise_queue(uint16_t max_queue_size) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) return NULL;
    uint8_t* data = (uint8_t*)malloc(max_queue_size*sizeof(uint8_t));
    if (data == NULL) {
        free(queue);
        return NULL;
    }
    queue->data = data;
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->array_length = max_queue_size;
    pthread_mutex_init(&queue->mutex, NULL);
    return queue;
}

void delete_queue(Queue* queue) {
    pthread_mutex_destroy(&queue->mutex);
    free(queue->data);
    free(queue);
}

Status dequeue(Queue* queue, uint8_t* data) {
    if (queue == NULL || data == NULL) return FAILURE;
    pthread_mutex_lock(&queue->mutex);
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return EMPTY;
    }
    *data = queue->data[queue->front]; 
    queue->front = (queue->front + 1) % queue->array_length;
    queue->size--;
    pthread_mutex_unlock(&queue->mutex);
    return SUCCESS;
}

Status enqueue(Queue* queue, uint8_t data) {
    if (queue == NULL) return FAILURE;
    pthread_mutex_lock(&queue->mutex);
    if (queue->size == queue->array_length) {
        pthread_mutex_unlock(&queue->mutex);
        return BUSY;
    }
    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->array_length;
    queue->size++;
    pthread_mutex_unlock(&queue->mutex);
    return SUCCESS;
}

uint8_t is_queue_full(Queue* queue) {
    if (queue == NULL) return 2; // Error
    pthread_mutex_lock(&queue->mutex);
    if (queue->size == queue->array_length) { // Queue Full
        pthread_mutex_unlock(&queue->mutex);
        return 1;
    } else { // Queue not full
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
}

uint8_t is_queue_empty(Queue* queue) {
    if (queue == NULL) return 2; // Error
    pthread_mutex_lock(&queue->mutex);
    if (queue->size == 0) { // Queue Empty
        pthread_mutex_unlock(&queue->mutex);
        return 1;
    } else { // Queue not full
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
}