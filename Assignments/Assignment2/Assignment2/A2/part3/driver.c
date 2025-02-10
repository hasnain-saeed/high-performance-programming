#include "linkedlist.h"

int main() {
    listNode_t *head = NULL;
    char command;
    int day;
    float min, max;

    do {
        printf("Enter command: ");
        scanf(" %c", &command);

        switch (command) {
        case 'A':
            if (scanf("%d %f %f", &day, &min, &max) != 3) {
                printf("Invalid input for add command\n");
                while (getchar() != '\n');
                break;
            }
            addNoteToList(&head, day, min, max);
            break;
        case 'D':
            if (scanf("%d", &day) != 1) {
                printf("Invalid input for delete command\n");
                while (getchar() != '\n');
                break;
            }
            deleteNoteFromList(&head, day);
            break;
        case 'P':
            printList(head);
            break;
        case 'Q': break;
        default:
            printf("Invalid command!\n");
            break;
        }
    } while (command != 'Q');

    // Cleanup before exit
    deleteList(&head);
    return 0;
}
