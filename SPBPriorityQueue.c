#include <malloc.h>
#include <assert.h>
#include "SPBPriorityQueue.h"


struct sp_bp_queue_t {
	BPQueueElement * elements;
	int size;
	int maxSize;
};

//Inner function indicating one queue element is greater than the other
bool isGreater(BPQueueElement el1, BPQueueElement el2) {
	// returns true if el1 > el2 (compares value and then index)
	return (el1.value > el2.value) || ((el1.value == el2.value) && (el1.index > el2.index));
}

SPBPQueue* spBPQueueCreate(int mSize) {
	if (mSize <=0)
		return NULL;

	// allocate priority queue struct
	SPBPQueue *res = (SPBPQueue*) malloc(sizeof(*res));
	if (res == NULL) { //Allocation failure
		return NULL;
	}

	// allocate elements array
	res->elements = (BPQueueElement*) malloc(mSize * sizeof(BPQueueElement));
	if (res->elements == NULL) { //Allocation failure - need to free(res)
		free(res);
		return NULL;
	}

	// initialize members
	res->maxSize = mSize;
	res->size = 0;
	return res;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source) {
	int i;
	if (source == NULL)
		return NULL;

	SPBPQueue *res = spBPQueueCreate(source->maxSize);
	if (res == NULL)
		return NULL;
	res->size = source->size;

	for (i=0; i<source->size; i++)
		res->elements[i] = source->elements[i];
	return res;
}

void spBPQueueDestroy(SPBPQueue* source) {
	if (source != NULL) {
		free(source->elements);
		free(source);
	}
}

void spBPQueueClear(SPBPQueue* source) {
	if (source != NULL)
		source->size = 0;
}

int spBPQueueSize(SPBPQueue* source) {
	assert(source != NULL);
	return source->size;
}

int spBPQueueGetMaxSize(SPBPQueue* source) {
	assert(source != NULL);
	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value) {
	if (source == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	BPQueueElement element = {index, value};

	if (spBPQueueIsFull(source)) { // is full
		// ignore elements with value greater than maximum
		if (isGreater(element, source->elements[0]))
			return SP_BPQUEUE_SUCCESS;

		// shift elements left
		int i=1;
		while (i<source->maxSize && isGreater(source->elements[i], element)) {
			source->elements[i-1] = source->elements[i];
			i++;
		}
		// insert new element
		source->elements[i-1] = element;
	}
	else { // is not full
		int i = source->size;
		// shift elements right
		if (!spBPQueueIsEmpty(source)) {
			while (i>0 && isGreater(element, source->elements[i-1])) {
				source->elements[i] = source->elements[i-1];
				i--;
			}
		}
		// insert new element
		source->elements[i] = element;
		source->size++;
	}

	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source) {
	if (source == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (spBPQueueIsEmpty(source))
		return SP_BPQUEUE_EMPTY;

	source->size--;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res) {
	if (source == NULL || res == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (source->size <= 0)
		return SP_BPQUEUE_EMPTY;

	res->index = source->elements[source->size-1].index;
	res->value = source->elements[source->size-1].value;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res) {
	if (source == NULL || res == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (source->size <= 0)
		return SP_BPQUEUE_EMPTY;

	res->index = source->elements[0].index;
	res->value = source->elements[0].value;
	return SP_BPQUEUE_SUCCESS;
}

double spBPQueueMinValue(SPBPQueue* source) {
	assert(source != NULL);
	assert(!spBPQueueIsEmpty(source));
	return source->elements[source->size-1].value;
}

double spBPQueueMaxValue(SPBPQueue* source) {
	assert(source != NULL);
	assert(!spBPQueueIsEmpty(source));
	return source->elements[0].value;
}

bool spBPQueueIsEmpty(SPBPQueue* source) {
	assert(source != NULL);
	return source->size == 0;
}

bool spBPQueueIsFull(SPBPQueue* source) {
	assert(source != NULL);
	return source->size == source->maxSize;
}
