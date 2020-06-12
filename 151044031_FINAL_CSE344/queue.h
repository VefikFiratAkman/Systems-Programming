// C program for array implementation of queue 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
  

/* INTERNETTEN ILK OLARAK INT QUEUEU OLARAK ALINMISTIR.
SONRA USTUNE TEKRARDAN QUEUE YAZILMISTIR. ARDINDAN
STRUCT QUEUE YA CEVRILMISTIR */

// A structure to represent a queue 
struct Queue 
{ 
    struct Node* front, *rear;
    int size;
    unsigned capacity; 
}; 
typedef struct Queue Queue;

struct Node
{
    char path[2048];
    int day;
    int hour;
    int minute;
    size_t size;
    int status;
    struct Node* next;

};

typedef struct Node Node;
  
// function to create a queue of given capacity.  
// It initializes size of queue as 0 
struct Queue* createQueue(unsigned capacity) 
{ 
  
    Queue* queue = (Queue*) malloc(sizeof(Queue)); 
    queue->front = NULL;
    queue->rear = NULL;

    queue->capacity = capacity; 
    queue->size = 0; 

    return queue; 
} 
  
// Queue is full when size becomes equal to the capacity  
int isFull(Queue* queue) 
{  return (queue->size == queue->capacity);  } 
  
// Queue is empty when size is 0 
int isEmpty(Queue* queue) 
{  return (queue->size == 0); } 
  
// Function to add an item to the queue.   
// It changes rear and size 
int enqueue(Queue* queue, Node* data) 
{ 
    if (isFull(queue)) 
        return -1; 

    Node *tmp;
    tmp = malloc(sizeof(Node));
    strcpy(tmp->path,data->path);

    tmp->day = data->day;
    tmp->hour = data->hour;
    tmp->minute = data->minute;
    tmp->status = data->status;
    

    tmp->size = data->size;
    tmp->next = NULL;
    if(!isEmpty(queue))
    {
        queue->rear->next = tmp;
        queue->rear = tmp;
    }
    else
    {
        queue->front = queue->rear = tmp;
    }
    ++queue->size;
    return 1;
} 
  
// Function to remove an item from queue.  
// It changes front and size 
Node * dequeue(Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return NULL; 


    Node *tmp;
    Node * pro  = (Node*) malloc(queue->capacity * sizeof(Node)); 

    strcpy(pro->path,queue->front->path);


    pro->day = queue->front->day;
    pro->hour = queue->front->hour;
    pro->minute = queue->front->minute;
    pro->size = queue->front->size;
    pro->status = queue->front->status;


    tmp = queue->front;
    queue->front = queue->front->next;
    queue->size--;

    free(tmp);

    return pro;

} 


Node * myindex(Queue* queue,int index){

    Node * tmp = (Node*) malloc(queue->capacity * sizeof(Node)); 
    Node * it = NULL;
    
    it = queue->front;

    if(index > queue->size){
        printf("OUT OF INDEX\n");
        return NULL;
    }
    for (int i = 0; i < index; ++i) {
        it = it->next;
    }

    tmp->day = it->day;
    tmp->hour = it->hour;
    tmp->minute = it->minute;
    tmp->size = it->size;
    tmp->status = it->status;
    strcpy(tmp->path,it->path);

    return tmp;
}
  
