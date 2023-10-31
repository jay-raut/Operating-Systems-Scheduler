/**
 *  list.h
 *
 *  Full Name: Jay Raut
 *  Course section: B
 *  Description of the program: list data structure containing the tasks in the system
 *  
 */
 
#include "process.h"

struct node {
    Process *process;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Process *process);
void insert_at_end (struct node **head, Process *newProcess);
void delete(struct node **head, Process *process);
void traverse(struct node *head);
