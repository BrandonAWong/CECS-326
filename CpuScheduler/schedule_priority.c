/*
 * Priority scheduling
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
    // traverse until list is empty
    while (head != NULL) {
        int max = -1;
        struct node *curr = head;
        struct node *priority_node;
        // find task with the highest priority
        while (curr != NULL) {
            if (max <= curr->task->priority) {
                max = curr->task->priority;
                priority_node = curr;
            }
            curr = curr->next;
        }
        run(priority_node->task, priority_node->task->burst);
        delete(&head, priority_node->task);
        free(priority_node->task);
        free(priority_node);
    }
}
