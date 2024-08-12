#include <stdio.h>
#include <stdlib.h>
#include "notif_queue.h"

void queue_push(Queue* q, Notif* n)
{
    QNode* node = (QNode*)malloc(sizeof(QNode));

    Notif* data = (Notif*)malloc(sizeof(Notif));
    *data = *n;

    node -> data = data;
    node -> next = NULL;

    if (q -> Front == NULL && q -> Rear == NULL){
        q -> Front = q -> Rear = node;
        q -> size++;
        return;
    };

    q -> Rear -> next = node;
    q -> Rear = node;
    q -> size++;
}

int queue_get(Queue* q, Notif* n)
{

    if (q-> Front == NULL){
        return 0;
    }

    QNode* ptr_node = q->Front;

    q -> size--;

    if (q->Front == q->Rear){
        q->Front = q->Rear = NULL;
    }
    else{
        q->Front = q->Front->next;
    }

    *n = *(ptr_node->data);

    free(ptr_node->data);
    free(ptr_node);
    return 1;
}
