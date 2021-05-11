/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 * Modified to store strings, 2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q =  malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if(q == NULL) return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->sizeL = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* How about freeing the list elements and the strings? */
    if(q == NULL) return;
    list_ele_t *cur = q->head;
    while(true){
      if(cur == NULL) break;
      free(cur->value);
      list_ele_t *nextCopy = cur->next;
      free(cur);
      cur = nextCopy;
    }
    /* Free queue structure */
    free(q);
    return;
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if(q == NULL) return false;

    list_ele_t *newh;
    /* What should you do if the q is NULL? */
    newh = malloc(sizeof(list_ele_t));
    if(newh == NULL) return false;
    /* Don't forget to allocate space for the string and copy it */
    char *newv;
    newv = malloc(strlen(s) + 1);
    if(newv == NULL){
      free(newh); // don't forget to free newh if you fail to allocate newv. Otherwise, newh will become orphan
      return false;
    }
    strcpy(newv,s);
    newh->value = newv;
    /* What if either call to malloc returns NULL? */
    newh->next = q->head;
    q->head = newh;
    q->sizeL++;
    if(q->tail == NULL) q->tail = newh;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */


    if(q == NULL) return false;

    list_ele_t *newt;
    /* What should you do if the q is NULL? */
    newt = malloc(sizeof(list_ele_t));
    if(newt == NULL) return false;
    /* Don't forget to allocate space for the string and copy it */
    char *newv;
    newv = malloc(strlen(s) + 1);
    if(newv == NULL){
      free(newt);
      return false;
    }
    strcpy(newv,s);
    newt->value = newv;
    newt->next = NULL;
    /* What if either call to malloc returns NULL? */
    if(q->tail == NULL){
      q->head = newt;
      q->tail = newt;
      q->sizeL++;
      return true;
    }
    else{
      q->tail->next = newt;
      q->tail = newt;
      q->sizeL++;
      return true;

    }
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* You need to fix up this code. */
    if(q == NULL||q->head == NULL) return false;
    list_ele_t *headCopy = q->head;
    if(sp != NULL){
      strncpy(sp,q->head->value,bufsize-1);
      sp[bufsize-1] = '\0';
    }
    if(q->head == q->tail) q->tail = q->head->next;
    q->head = q->head->next;
    q->sizeL--;
    free(headCopy->value);
    free(headCopy);
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if(q == NULL||q->head == NULL) return 0;
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    else return q->sizeL;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* You need to write the code for this function */
    if(q == NULL||q->head == NULL) return;
    list_ele_t *cur = q->head;
    list_ele_t *nextCopy;
    list_ele_t *newNext = NULL;
    q->tail = q->head;
    while(true){
      if(cur == NULL) break;
      nextCopy = cur->next;
      cur->next = newNext;
      newNext = cur;
      cur = nextCopy;
    }
    q->head = newNext;
    return;
}

