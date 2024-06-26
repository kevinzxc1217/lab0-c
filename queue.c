#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

void q_merge_2_list(struct list_head *list_1, struct list_head *list_2);

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
    list_del_init(&(first -> list));
    
    if (sp) {
        strncpy(sp, first -> value, bufsize -1);
        sp[bufsize - 1] = '\0';
    }
    
    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{

    if(!head || list_empty(head))
        return NULL;
        
    element_t *last = list_last_entry(head, element_t, list);
    list_del_init(&(last -> list));
    
    if (sp) {
        strncpy(sp, last -> value, bufsize -1);
        sp[bufsize - 1] = '\0';
    }
    
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
    if(!head)
        return;
    struct list_head *node = head;
    list_for_each(node,head){
        if(node -> next != head)
            list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {
    struct list_head *cur = head;
    struct list_head *next = head -> next;
    struct list_head *prev = head -> prev;
    
    while(next != prev){
      cur -> prev = next;
      cur -> next = prev;
      prev = cur;
      cur = next;
      next = next -> next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    
    struct list_head *list_cur = head;
    struct list_head *tmp_end = head;
    struct list_head *tmp_next = head;
    struct list_head tmp_beg;
    INIT_LIST_HEAD(&tmp_beg);
    int cnt = 0;
    while(tmp_end -> next != head){
        if(cnt == k){
            tmp_next = tmp_end -> next;
            list_cut_position(&tmp_beg, list_cur, tmp_end);
            q_reverse(&tmp_beg);
            list_splice_init(&tmp_beg, list_cur);
            list_cur = tmp_next -> prev;
            tmp_end = list_cur;
            cnt = 0;
        }
        else{ 
            tmp_end = tmp_end -> next;
            cnt = cnt + 1;
        }
    }
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {
    
    if(!head || list_is_singular(head) || list_empty(head))
        return ;
    struct list_head *list_cur = head;
    struct list_head *fast = head -> next;
    struct list_head *slow = head -> next;
    struct list_head list_new;
    INIT_LIST_HEAD(&list_new);
    while(fast -> next != head && fast -> next -> next != head){
        fast = fast -> next -> next;
        slow = slow -> next;
    }
    list_cut_position(&list_new, list_cur, slow); 
    q_sort(&list_new, false);
    q_sort(head, false);
    q_merge_2_list(head, &list_new);
    
    return ;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if(!head)
      return 0;
      
    int len = 0;
    struct list_head *list_rem;
    struct list_head *list_cur  = head -> next;
    struct list_head *list_next = head -> next -> next;
    while(list_next != head){
        len ++;
        while(list_cur != head &&
            strcmp(list_entry(list_cur, element_t, list) -> value, list_entry(list_next, element_t, list) -> value) >= 0){
            list_rem = list_cur;
            list_cur = list_cur ->prev;
            list_del_init(list_rem);
            q_release_element(list_entry(list_rem,  element_t, list));
            len --;
        }
        list_cur = list_next;
        list_next = list_cur -> next;
    }
    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if(!head)
      return 0;
      
    int len = 0;
    struct list_head *list_rem;
    struct list_head *list_cur  = head -> next;
    struct list_head *list_next = head -> next -> next;
    while(list_next != head){
        len ++;
        while(list_cur != head &&
            strcmp(list_entry(list_cur,  element_t, list) -> value, list_entry(list_next, element_t, list) -> value) < 0){
            list_rem = list_cur;
            list_cur = list_cur ->prev;
            list_del_init(list_rem);
            q_release_element(list_entry(list_rem,  element_t, list)); 
            len --;
        }
        list_cur = list_next;
        list_next = list_cur -> next;
    }
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    queue_contex_t *contex_cur = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *contex_next = list_entry(head -> next ->next, queue_contex_t, chain);
    struct list_head *list_cur = head -> next;
    struct list_head *list_next = list_cur -> next;
    while(list_next != head){
        contex_next = list_entry(list_next, queue_contex_t, chain);
        q_merge_2_list(contex_cur -> q, contex_next -> q);
        list_next = list_next -> next;
    }
    int contex_size = q_size(contex_cur -> q);
    
    return contex_size;
}

void q_merge_2_list(struct list_head *list_1, struct list_head *list_2){
    
    struct list_head list_result;
    INIT_LIST_HEAD(&list_result);
    while(!list_empty(list_1) && !list_empty(list_2)){
        element_t *elem_1 = list_entry(list_1->next, element_t, list);
        element_t *elem_2 = list_entry(list_2->next, element_t, list);
         
        if(strcmp(elem_1 -> value, elem_2 -> value) < 0)
            list_move_tail(list_1 -> next, &list_result);
        else
            list_move_tail(list_2 -> next, &list_result);
    }
    if(!list_empty(list_1))
        list_splice_tail_init(list_1, &list_result);
    else if(!list_empty(list_2))
        list_splice_tail_init(list_2, &list_result);
    list_splice_tail_init(&list_result, list_1);
}