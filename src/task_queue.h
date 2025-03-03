#ifndef TASK_QUEUE_HEADER
#define TASK_QUEUE_HEADER

#define QEMPTY (-1)
#define QERROR (-2)

typedef struct task_node {
    int acceptfd;
    struct task_node *next_node;
} task_node;

typedef struct task_queue {
    int size;
    task_node *head;
    task_node *tail;
} task_queue;

task_queue create_task_queue();
int append_task(task_queue *queue, int acceptfd); 
int dequeue_task(task_queue *queue); 
int is_empty(task_queue *queue);
void del_queue(task_queue *queue); 

#endif
