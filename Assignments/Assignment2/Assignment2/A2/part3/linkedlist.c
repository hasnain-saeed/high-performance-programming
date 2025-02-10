#include "linkedlist.h"

// validates if day/index is within valid range (1-31)
int isValidDay(int day) {
    return (day >= 1 && day <= 31);
}

// creates a new node with given data
listNode_t* createNode(int day, float min, float max){
    listNode_t* node = (listNode_t*)malloc(sizeof(listNode_t));
    node->day = day;
    node->min = min;
    node->max = max;
    node->next = NULL;
    return node;
}

// adds a new node or updates existing node with temperature data
void addNoteToList(listNode_t** head, int day, float min, float max) {
    // validate day range
    if (!isValidDay(day)) {
        printf("Error: Day must be between 1 and 31\n");
        return;
    }

    // empty list case
    if (*head == NULL){
        *head = createNode(day, min, max);
        return;
    }

    // insert at beginning if day is smaller
    if(day < (*head)->day){
        listNode_t* newHead = createNode(day, min, max);
        newHead->next = *head;
        *head = newHead;
        return;
    }

    // update existing day
    else if(day == (*head)->day) {
        (*head)->min = min;
        (*head)->max = max;
        return;
    }

    // recursively add to rest of list
    else {
        addNoteToList((&(*head)->next), day, min, max);
    }
}

// deletes node with specified day
void deleteNoteFromList(listNode_t** head, int day){
    if(*head == NULL){
        return;
    }

    if ((*head)->day == day){
        listNode_t* temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    listNode_t* temp = *head;

    while (temp->next != NULL){
        if (temp->next->day == day) {
            listNode_t* node = temp->next;
            temp->next = temp->next->next;
            free(node);
            return;
        }
        temp = temp->next;
    }
}

// prints the temperature data in tabular form
void printList(listNode_t* head) {
    printf("%-6s %-10s %-10s\n", "day", "min", "max");
    while (head != NULL) {
        printf("%-6d %9.6f %9.6f\n", head->day, head->min, head->max);
        head = head->next;
    }
}

// recursively deletes all nodes and frees memory
void deleteList(listNode_t** head){
    if(*head == NULL){
        return;
    }
    deleteList(&((*head)->next));
    free(*head);
    *head = NULL;
}
