#include "list.h"
#include <stdio.h>

#define NUM_LIST_NODES 100
#define NUM_LIST_HEADS 10

#define SUCCESS 0
#define FAILED -1

NODE nodes[NUM_LIST_NODES];
LIST heads[NUM_LIST_HEADS];

NODE* nodeAvailable;
LIST* headAvailable;

int initialized = 0;
void init() {
	// init nodes
	if(NUM_LIST_NODES > 0) {
		int lastNode = NUM_LIST_NODES - 1;
		// link all nodes
		for(int i = 0; i < lastNode; i++) {
			nodes[i].next = &nodes[i+1];
		}

		nodeAvailable = &nodes[0];
		nodes[lastNode].next = NULL;
	} 
	else {
		nodeAvailable = NULL;
	}

	// init heads
	if(NUM_LIST_HEADS > 0) {
		int lastHead = NUM_LIST_HEADS - 1;
		// link all heads
		for(int i = 0; i < lastHead; i++) {
			// convert to node
			heads[i].head = (NODE*)&heads[i+1];
		}

		headAvailable = &heads[0];
		heads[lastHead].head = NULL;
	} 
	else {
		headAvailable = NULL;
	}

	initialized = 1;
}

void resetListCurrentState(LIST* list) {
	if(list == NULL) {
		return;
	}

	list->current = NULL;
	list->currentBeforeStart = 0;
	list->currentBeyondEnd = 0;
}

void resetListToDefaultValues(LIST* list) {
	if(list == NULL) {
		return;
	}

	list->head = NULL;
	list->tail = NULL;
	list->length = 0;

	resetListCurrentState(list);
}

void resetNodeFields(NODE* node) {
	node->next = NULL;
	node->prev = NULL;
	node->item = NULL;
}

// Helper method
// Gets next HEAD from pool
LIST* getNextAvailableHead() {
	if(headAvailable == NULL) {
		printf("Error: No available heads - maximum capacity reached. NUM_LIST_HEADS = %d\n", NUM_LIST_HEADS);
		return NULL;
	}

	LIST* head = headAvailable;
	headAvailable = (LIST*)headAvailable->head; 

	resetListToDefaultValues(head);

	return head;
}

// Helper method
// Gets next NODE from pool
NODE* getNextAvailableNode() {
	if(nodeAvailable == NULL) {
		printf("Error: No available nodes - maximum capacity reached. NUM_LIST_NODES = %d\n", NUM_LIST_NODES);
		return NULL;
	}

	NODE* node = nodeAvailable;
	nodeAvailable = nodeAvailable->next;

	if(nodeAvailable != NULL) { 
		nodeAvailable->prev = NULL;
	}

	resetNodeFields(node);

	return node;
}

// Helper method
// Returns HEAD back to the pool
void releaseHead(LIST* releaseHead) {
	resetListToDefaultValues(releaseHead);

	if(headAvailable == NULL) {
		headAvailable = releaseHead;
		return;
	}

	headAvailable->tail = (NODE*)releaseHead;
	releaseHead->head = (NODE*)headAvailable;

	headAvailable = releaseHead;
}

// Helper method
// Returns NODE back to the pool
void releaseNode(NODE* releaseNode) {
	resetNodeFields(releaseNode);

	if(nodeAvailable == NULL) {
		nodeAvailable = releaseNode;
		return;
	}

	nodeAvailable->prev = releaseNode;
	releaseNode->next = nodeAvailable;

	nodeAvailable = releaseNode;
}

// Helper method
// Return 1 if list is NULL
// Return 0 if list valid
int listIsNull(LIST* list) {
	if(list == NULL) {
		printf("Error: List NULL\n");
		return 1;
	}
	return 0;
}

/// 
/// LIST* ListCreate()
/// makes a new, empty list, and returns its reference on
/// success. Returns a NULL pointer on failure.
///
LIST* ListCreate() {
	if(!initialized) {
		init();
	}

	LIST* list = (LIST*)getNextAvailableHead();

	if(list == NULL) {
		return NULL;
	}

	return list;
}

///
/// int ListCount(LIST* list)
/// returns the number of items in list.
/// return -1 if NULL
///
int ListCount(LIST* list) {
	if(listIsNull(list)) {
		return FAILED;
	}
	return list->length;
}

///
/// void *ListFirst(LIST* list)
/// returns a pointer to the first item in list and makes
/// the first item the current item.
///
void *ListFirst(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}
	resetListCurrentState(list);
	list->current = list->head;
	return ListCurr(list);
}

///
/// void *ListLast(LIST* list)
/// returns a pointer to the last item in list and makes
/// the last item the current one.
///
void *ListLast(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}
	resetListCurrentState(list);
	list->current = list->tail;
	return ListCurr(list);
}

///
/// void *ListNext(LIST* list)
/// advances list's current item by one, and returns a
/// pointer to the new current item. If this operation advances the current item beyond
/// the end of the list, a NULL pointer is returned.
///
void *ListNext(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}

	if(list->currentBeforeStart) {
		return ListFirst(list);
	}

	if(list->current == NULL) {
		return NULL;
	}

	if(list->currentBeyondEnd) {
		return NULL;
	}

	if(list->current->next == NULL) {
		resetListCurrentState(list);
		list->currentBeyondEnd = 1;
		return NULL;
	}

	// set to next
	list->current = list->current->next;
	return ListCurr(list);
}

///
/// void *ListPrev(LIST* list)
/// backs up list's current item by one, and returns a
/// pointer to the new current item. If this operation backs up the current item beyond
/// the start of the list, a NULL pointer is returned.
///
void *ListPrev(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}

	if(list->currentBeyondEnd) {
		return ListLast(list);
	}

	if(list->current == NULL) {
		return NULL;
	}

	if(list->currentBeforeStart) {
		return NULL;
	}

	if(list->current->prev == NULL) {
		resetListCurrentState(list);
		list->currentBeforeStart = 1;
		return NULL;
	}

	// set to prev
	list->current = list->current->prev;
	return ListCurr(list);
}

///
/// void *ListCurr(LIST* list)
/// returns a pointer to the current item in list
///
void *ListCurr(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}

	if(list->current == NULL) {
		return NULL;
	}

	return list->current->item;
}

// Helper method:
// Returns new NODE if list is valid and node was allocated properly
NODE* setupNodeForAdd(LIST* list, void* item) {
	if(listIsNull(list)) {
		return NULL;
	}

	NODE* node = getNextAvailableNode();

	if(node == NULL) {
		return NULL;
	}

	node->item = item;

	return node;
}

// Helper method:
// Sets list->current to the new current node
// Increments the length of the list
// Returns success
int successfullyAddedNode(LIST* list, NODE* node) {
	list->current = node;
	list->length++;
	return SUCCESS;
}

// Helper method:
// Adds the first item to the list, making it head, tail, and current.
int createFirstNode(LIST* list, void* item) {
	NODE* node = setupNodeForAdd(list, item);
	if(node == NULL) {
		return FAILED;
	}

	list->head = node;
	list->tail = node;

	return successfullyAddedNode(list, node);
}

///
/// int ListAdd(LIST* list, void* item)
/// adds the new item to list directly after the
/// current item, and makes item the current item. If the current pointer is before the
/// start of the list, the item is added at the start. If the current pointer is beyond the
/// end of the list, the item is added at the end. Returns 0 on success, -1 on failure.
///
int ListAdd(LIST* list, void* item) {
	if(listIsNull(list)) {
		return FAILED;
	}

	if(list->length == 0) {
		return createFirstNode(list, item);
	}

	if(list->currentBeyondEnd) {
		return ListAppend(list, item);
	}

	if(list->currentBeforeStart) {
		return ListPrepend(list, item);
	}

	NODE* node = setupNodeForAdd(list, item);
	if(node == NULL) {
		return FAILED;
	}

	if(list->tail == list->current) {
		list->tail = node;
	}

	// update the node that is being added
	node->next = list->current->next;
	node->prev = list->current;

	// update the current selected node
	if(list->current->next != NULL) {
		list->current->next->prev = node;
	}

	list->current->next = node;

	return successfullyAddedNode(list, node);
}

///
/// int ListInsert(LIST* list, void* item)
/// adds item to list directly before the current
/// item, and makes the new item the current one. If the current pointer is before the
/// start of the list, the item is added at the start. If the current pointer is beyond the
/// end of the list, the item is added at the end. Returns 0 on success, -1 on failure.
///
int ListInsert(LIST* list, void* item) {
	if(listIsNull(list)) {
		return FAILED;
	}

	if(list->length == 0) {
		return createFirstNode(list, item);
	}

	if(list->currentBeyondEnd) {
		return ListAppend(list, item);
	}

	if(list->currentBeforeStart) {
		return ListPrepend(list, item);
	}

	NODE* node = setupNodeForAdd(list, item);
	if(node == NULL) {
		return FAILED;
	}

	if(list->head == list->current) {
		list->head = node;
	}

	// update the node that is being inserted
	node->next = list->current;
	node->prev = list->current->prev;

	// update the current selected node
	if(list->current->prev != NULL) {
		list->current->prev->next = node;
	}

	list->current->prev = node;

	return successfullyAddedNode(list, node);
}

///
/// int ListAppend(LIST* list, void* item)
/// adds item to the end of list, and makes the
/// new item the current one. Returns 0 on success, -1 on failure.
///
int ListAppend(LIST* list, void* item) {
	if(listIsNull(list)) {
		return FAILED;
	}

	if(list->length == 0) {
		return createFirstNode(list, item);
	}

	NODE* node = setupNodeForAdd(list, item);
	if(node == NULL) {
		return FAILED;
	}
	
	resetListCurrentState(list);

	node->prev = list->tail;
	list->tail->next = node;
	list->tail = node;

	return successfullyAddedNode(list, node);
}

///
/// int ListPrepend(LIST* list, void* item)
/// adds item to the front of list, and makes
/// the new item the current one. Returns 0 on success, -1 on failure
///
int ListPrepend(LIST* list, void* item) {
	if(listIsNull(list)) {
		return FAILED;
	}

	if(list->length == 0) {
		return createFirstNode(list, item);
	}

	NODE* node = setupNodeForAdd(list, item);
	if(node == NULL) {
		return FAILED;
	}

	resetListCurrentState(list);

	node->next = list->head;
	list->head->prev = node;
	list->head = node;

	return successfullyAddedNode(list, node);
}

///
/// void *ListRemove(LIST* list)
/// Return current item and take it out of list. Make
/// the next item the current one.
///
void *ListRemove(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}

	if(list->length == 0) {
		return NULL;
	}

	if(list->current == NULL) {
		return NULL;
	}

	void* itemRemoved = list->current->item;
	NODE* nodeRemoved = list->current;

	if(list->length == 1) {
		releaseNode(nodeRemoved);
		resetListToDefaultValues(list);
		return itemRemoved;
	}

	if(list->head == list->current) {
		list->head = list->head->next;
		list->head->prev = NULL;

		releaseNode(nodeRemoved);

		list->length--;
		ListFirst(list);

		return itemRemoved;
	} 

	else if(list->tail == list->current) {
		list->tail = list->tail->prev;
		list->tail->next = NULL;
	}

	if(list->current->next != NULL) {
		list->current->next->prev = list->current->prev;
	}

	if(list->current->prev != NULL) {
		list->current->prev->next = list->current->next;
	}

	releaseNode(nodeRemoved);

	list->length--;
	ListNext(list);

	return itemRemoved;
}

///
/// void *ListTrim(LIST* list)
/// Return last item and take it out of list. Make the
// new last item the current one.
///
void *ListTrim(LIST* list) {
	if(listIsNull(list)) {
		return NULL;
	}

	// set current node to last node
	ListLast(list);

	// remove current node (last node)
	void* itemRemoved = ListRemove(list);

	// set current node to new last node
	ListLast(list);

	return itemRemoved;
}

///
/// void ListConcat(LIST* list1, LIST* list2)
/// adds list2 to the end of list1. The
/// current pointer is set to the current pointer of list1. List2 no longer exists after the
/// operation
///
void ListConcat(LIST* list1, LIST* list2) {
	if(listIsNull(list1)) {
		return;
	}

	if(listIsNull(list2)) {
		return;
	}

	if(list2->length == 0) {
		releaseHead(list2);
		return;
	}

	// copy list2 to list1
	if(list1->length == 0) {
		resetListToDefaultValues(list1);
		list1->head = list2->head;
		list1->tail = list2->tail;
		list1->length = list2->length;
		releaseHead(list2);
		return;
	} 

	if(list1->tail != NULL && list2->head != NULL) {
		list1->tail->next = list2->head;
		list2->head->prev = list1->tail;
	}

	list1->tail = list2->tail;

	int newLength = list1->length + list2->length;
	list1->length = newLength;

	releaseHead(list2);
}

///
/// void ListFree(LIST* list, void (*itemFree)(void* itemToBeFreed))
/// delete list. itemFree is a pointer to a
/// routine that frees an item. It should be invoked (within ListFree) 
///
void ListFree(LIST* list, void (*itemFree)(void* itemToBeFreed)) {
	if(listIsNull(list)) {
		return;
	}

	ListFirst(list);
	
	while(ListCurr(list) != NULL) {
		(*itemFree)(ListCurr(list));
		ListRemove(list);
	}

	releaseHead(list);
}

///
/// void *ListSearch(LIST* list, int (*comparator)(void* item, void* comparisonArg), void* comparisonArg);
/// searches list starting at the current item until the end is reached or a match is
/// found. In this context, a match is determined by the comparator parameter. This
/// parameter is a pointer to a routine that takes as its first argument an item pointer,
/// and as its second argument comparisonArg. Comparator returns 0 if the item and
/// comparisonArg don't match, or 1 if they do. Exactly what constitutes a match is
/// up to the implementor of comparator. If a match is found, the current pointer is
/// left at the matched item and the pointer to that item is returned. If no match is
/// found, the current pointer is left beyond the end of the list and a NULL pointer is
/// returned.
///
void *ListSearch(LIST* list, int (*comparator)(void* item, void* comparisonArg), void* comparisonArg) {
	while(ListCurr(list) != NULL) {
		if((*comparator)(ListCurr(list), comparisonArg)) {
			return ListCurr(list);
		}

		ListNext(list);
	}

	return NULL;
}