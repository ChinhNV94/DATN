#include "myDataType.h"
#include <stdbool.h>

#define     QUEUE_SIZE          256

typedef struct _queue
{
    uint8 data[QUEUE_SIZE];
    uint8 data_size = 0;
    uint8 get_index = 0;
    uint8 put_index = 0;
}queue;

extern queue RxQueue;
extern queue TxQueue;

bool queue_empty(queue *q);
uint8 queue_getByte(queue* q);
void queue_putByte(queue* q);