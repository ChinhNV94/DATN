#ifndef MYQUEUE_H
#define MYQUEUE_H

/*************************************************************************/
/* Include */

#include "data_type.h"
#include <stdbool.h>

/*************************************************************************/
/* Define & Typedef */

#define     QUEUE_SIZE          255

typedef struct _queue
{
    uint8 data[QUEUE_SIZE];
    uint8 data_size;
    uint8 get_index;
    uint8 put_index;
}queue;

/*************************************************************************/
/* Function */

bool queue_empty(queue *q);
uint8 queue_getByte(queue* q);
void queue_putByte(queue* q, uint8 byte);

#endif
