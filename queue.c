#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head)); 
    if(!head)
        return NULL;
        
    head -> next = head;
    head -> prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {

    if(!head)
        return;
    element_t *cur, *safe;
    
    list_for_each_entry_safe (cur, safe, head, list) {
        q_release_element(cur);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if(!head)
        return false;
    
    element_t* newnode = malloc(sizeof(element_t));
    if(!newnode)
        return false;
        
    list_add(&newnode -> list,head);
    newnode -> value = malloc((strlen(s)+1)*sizeof(char));
    if(!newnode -> value)
        return false;
        
    strcpy(newnode->value,s);
    
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if(!head)
        return false;

    element_t *newnode = malloc(sizeof(element_t));
    if(!newnode)
        return false;
    list_add_tail(&newnode -> list,head);
    
    newnode -> value = malloc((strlen(s)+1)*sizeof(char));
    if(!newnode -> value)
        return false;
    strcpy(newnode->value,s);
    
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if(!head || list_empty(head))
        return NULL;
    
    element_t *first = list_first_entry(head, element_t, list);
    strncpy(sp, first -> value, bufsize -1);
    sp[bufsize - 1] = '\0';
    list_del_init(&(first -> list));
    
    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if(!head || list_empty(head))
        return NULL;
        
    element_t *last = list_last_entry(head, element_t, list);    
    strcpy(sp,last->value);
    sp[bufsize - 1] = '\0';
    list_del_init(&(last -> list));
    
    return last;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *fast = head -> next;
    struct list_head *slow = head -> next;
    
    if(head == NULL)
        return false;

    while(fast -> next != head && fast -> next -> next != head){
        slow = slow -> next;
        fast = fast -> next -> next;
    }
    
    list_del_init(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if(!head)
        return false;
    
    struct list_head *list_cur  = head -> next;
    struct list_head *list_next = list_cur -> next;
    
    while(list_next != head){
        if(strcmp(list_entry(list_cur,  element_t, list) -> value, list_entry(list_next, element_t, list) -> value) == 0){
            while(list_next != head &&
                  strcmp(list_entry(list_cur,  element_t, list) -> value, list_entry(list_next, element_t, list) -> value) == 0){
                list_del_init(list_cur);
                q_release_element(list_entry(list_cur, element_t, list));
                list_cur = list_next;
                list_next = list_next -> next;
            }
            list_del_init(list_cur);
            q_release_element(list_entry(list_cur, element_t, list));
        }
        if(list_next != head){
            list_cur = list_next;
            list_next = list_next -> next;
        }

    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
