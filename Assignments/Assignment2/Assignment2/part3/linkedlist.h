#include <stdio.h>
#include <stdlib.h>

// structure to store temperature data for a day
typedef struct listNode
{
   int day;
   float min;
   float max;
   struct listNode *next;
} listNode_t;

// function declarations
listNode_t* createNode(int, float, float);
void addNoteToList(listNode_t**, int, float, float);
void deleteNoteFromList(listNode_t**, int);
void printList(listNode_t*);
void deleteList(listNode_t**);
int isValidDay(int day);
