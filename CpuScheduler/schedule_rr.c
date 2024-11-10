/**
 * Round-robin scheduling
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

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

    // keep iterating until all tasks have completed
    while (head != NULL) {
        struct node *curr = head;
        while (curr != NULL) {
            // run for either a quantum, or remaining burst
            run(curr->task, (curr->task->burst > QUANTUM) ? QUANTUM : curr->task->burst);
            curr->task->burst -= QUANTUM;

            struct node* next = curr->next;
            if (curr->task->burst <= 0) {
                delete(&head, curr->task);
                free(curr->task);
                free(curr);
            }
            curr = next;
        }
    }
}
