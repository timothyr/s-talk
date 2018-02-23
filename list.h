#ifndef LIST_H
#define LIST_H

typedef struct node {
	struct node* next;
	struct node* prev;
	void* item;
} NODE;

typedef struct list {
	NODE* head;
	NODE* tail;
	NODE* current;
	int currentBeforeStart;
	int currentBeyondEnd;
	int length;
} LIST;

LIST *ListCreate();

int ListCount(LIST* list);

// Find item in List

void *ListFirst(LIST* list);

void *ListLast(LIST* list);

void *ListNext(LIST* list);

void *ListPrev(LIST* list);

void *ListCurr(LIST* list);

// Modify List

int ListAdd(LIST* list, void* item);

int ListInsert(LIST* list, void* item);

int ListAppend(LIST* list, void* item);

int ListPrepend(LIST* list, void* item);

void *ListRemove(LIST* list);

void *ListTrim(LIST* list);

// Misc

void ListConcat(LIST* list1, LIST* list2);

void ListFree(LIST* list, void (*itemFree)(void* itemToBeFreed));

void *ListSearch(LIST* list, int (*comparator)(void* item, void* comparisonArg), void* comparisonArg);

#endif