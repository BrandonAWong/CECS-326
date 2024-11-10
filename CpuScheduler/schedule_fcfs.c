/**
 * FCFS scheduling
 */
 
#include <stdlib.h>
#include <stdio.h>

#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *head;

void add(char* name, int priority, int burst) {
    static int task_id = 0;
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->tid = ++task_id;
    task->priority = priority;
    task->burst = burst;
    insert(&head, task);
}

void schedule() {
    struct node *curr = head;
    struct node *next = NULL;
    struct node *prev = NULL;
 
    // Reverse the linked list. #206 know your lore yo
    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    head = prev;

    struct node *temp;
    while (head != NULL) {
        run(head->task, head->task->burst);
        temp = head;
        head = head->next;
        free(temp->task);
        free(temp);
    }
}
