#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new empty priority queue in the memory.
 * @param maxSize - the maximum size of the queue.
 *
 * @return
 * NULL in case allocation failure occurred OR maxSize <=0
 * Otherwise, a pointer to the queue
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * Allocates a new priority queue in the memory, of
 * the same size as the given queue, and copies
 * the elements of the given queue into the
 * new priority queue.
 *
 * @param source - the queue to be copied
 *
 * @return
 * NULL in case allocation failure occurred OR given queue is NULL
 * Otherwise, a pointer to the queue
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Free all memory resources associate with the priority queue.
 * If queue is NULL, nothing is done.
 * @param source - the queue to be destroyed
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * Empties the priority queue.
 * Sets the size of the queue to 0.
 * If queue is NULL, nothing is done.
 * @param source - the queue to be cleared
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * A getter for the current size of the queue
 *
 * @param source - the source queue
 * @assert source != NULL
 * @return
 * the number of elements in the queue
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * A getter for the maximum size of the queue
 *
 * @param source - the source queue
 * @assert source != NULL
 * @return
 * the maximum size the queue can reach
 */
 int spBPQueueGetMaxSize(SPBPQueue* source);

 /**
  * Inserts a new element into the queue so that the
  * elements are sorted in ascending order by value (and index)
  * (the element with the lowest value is the first).
  * If the queue is full before the insertion, after the
  * insertion discards the element with the highest value.
  *
  * @param source - the source queue
  * @param index - the index of the new element
  * @param value - the value of the new element
  *
  * @return SP_BPQUEUE_INVALID_ARGUMENT in case the queue is NULL
  *         SP_BPQUEUE_SUCCESS otherwise
  */
 SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * Removes the first element in the queue
 * (the element with the lowest value)
 * If the queue is empty does nothing.
 *
 * @param source - the source queue
 *
 * @return SP_BPQUEUE_INVALID_ARGUMENT in case the queue is NULL
 *         SP_BPQUEUE_EMPTY in case the queue is empty
 *         SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Returns a copy of the element with the lowest value in the queue
 *
 * @param source - the source queue
 * @param res - the address in which the copied element will be stored
 *
 * @return SP_BPQUEUE_INVALID_ARGUMENT in case the queue is NULL or res is NULL
 *         SP_BPQUEUE_EMPTY in case the queue is empty
 *         SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Returns a copy of the element with the highest value in the queue
 *
 * @param source - the source queue
 * @param res - the address in which the copied element will be stored
 *
 * @return SP_BPQUEUE_INVALID_ARGUMENT in case the queue is NULL or res is NULL
 *         SP_BPQUEUE_EMPTY in case the queue is empty
 *         SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * Returns the value of the element with the lowest value in the queue
 *
 * @param source - the source queue
 * @assert source != NULL
 * @assert queue is not empty
 * @return
 * the value of the element with the lowest value in the queue.
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * Returns the value of the element with the highest value in the queue
 *
 * @param source - the source queue
 * @assert source != NULL
 * @assert queue is not empty
 * @return
 * the value of the element with the highest value in the queue.
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * Returns true if the queue is empty
 *
 * @param source - the source queue
 * @assert source != NULL
 * @return
 * True if the queue is empty, otherwise False
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * Returns true if the queue is full
 *
 * @param source - the source queue
 * @assert source != NULL
 * @return
 * True if the queue is full, otherwise False
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
