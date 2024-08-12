/*
 * queue
 *
 *  Created on: Jul 12, 2024
 *      Author: GoReNeY
 */

#ifndef INC_NOTIF_QUEUE_H_
#define INC_NOTIF_QUEUE_H_

#include <stddef.h>

typedef struct {
    short number;
    int state;
} Notif;

typedef struct QNode {
    Notif* data;
    struct QNode* next;
} QNode;

typedef struct {
    QNode* Rear;
    QNode* Front;
    size_t size;
} Queue;

void queue_push(Queue* q, Notif* n);
int queue_get(Queue* q, Notif* n);

#endif /* INC_NOTIF_QUEUE_H_ */
