#include "task_queue.h"

#include <stdlib.h>

task_queue create_task_queue() {
    task_queue q;
    q.head = NULL;
    q.tail = NULL;
    q.size = 0;
    return q;
}

int append_task(task_queue *queue, int acceptfd) {
    if (queue == NULL) {
        return QERROR;
    }

    //create task node
    task_node *tasknode = (task_node*) malloc(sizeof(task_node));
    if (tasknode == NULL) {
        return QERROR;
    }
    tasknode->acceptfd = acceptfd;
    tasknode->next_node = NULL;

    //update tail
    if (queue->tail != NULL) {
        queue->tail->next_node = tasknode;
        queue->tail = tasknode;
    } else {
        queue->head = queue->tail = tasknode;
    }
    queue->size++;
    return 0;
}

int dequeue_task(task_queue *queue) {
    if (queue == NULL) {
        return QERROR;
    }

    //update head
    if (queue->head == NULL) {
        return QEMPTY;
    }

    task_node *tmp_head = queue->head;
    int poppedfd = tmp_head->acceptfd;
    queue->head = queue->head->next_node;
    if (queue->head == NULL) {
        queue->head = queue->tail = NULL;
    }

    queue->size--;
    free(tmp_head);
    return poppedfd;
}

int is_empty(task_queue *queue) {
    int s;
    if (queue->head == NULL) {
         s = 1;
    } else {
        s = 0;
    }
    return s;
}

void del_queue(task_queue *queue) {
    while(queue->head != NULL){ 
       dequeue_task(queue); 
    }
    free(queue);
}
