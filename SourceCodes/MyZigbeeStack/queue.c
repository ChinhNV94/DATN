/*************************************************************************/
/* Include */

#include "queue.h"

/*************************************************************************/
/* Function */

bool queue_empty(queue *q)
{
    bool reVal = false;
    if(!q->data_size)
    {
        reVal = true;
        q->get_index = 0;
        q->put_index = 0;
    }
    return reVal;
}

uint8 queue_getByte(queue* q)
{
    uint8 reVal = 0;

    if(q->get_index >= QUEUE_SIZE)
    {
        q->get_index %= QUEUE_SIZE;
    }
    if(!queue_empty(q))
    {
        reVal = q->data[q->get_index];
        q->get_index++;
        q->data_size--;
    }
    return reVal;
}
void queue_putByte(queue* q, uint8 byte)
{
    if(q->put_index >= QUEUE_SIZE)
    {
        q->put_index %= QUEUE_SIZE;
    }
    if(q->data_size < QUEUE_SIZE)
    {
        q->data[q->put_index] = byte;
        q->put_index++;
        q->data_size++;
    }
}
