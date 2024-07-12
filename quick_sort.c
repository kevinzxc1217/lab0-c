#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"


typedef struct __node {
    struct list_head list;
    long value;
} node_t;



/* Verify if list is order */
static bool list_is_ordered(struct list_head *head)
{       
    struct list_head *cur = head -> next;

    while(cur != head && cur-> next != head){
        node_t *n_cur = list_entry(cur,node_t,list);
        node_t *n_next = list_entry(cur->next,node_t,list);
        if(n_cur->value < n_next->value)
            return false;
        cur = cur -> next;
    }

    return true;
}

void shuffle(int *array, int n)
{
    if (n <= 0)
        return;

    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}



void *list_construct(struct list_head *head, int n){

    if(!head)
        return false;
    node_t* node = malloc(sizeof(node_t));

    list_add_tail(&node -> list, head);
    node -> value = n;
}


void list_free(struct list_head *head) 
{
    if(!head)
        return ;
    node_t *cur, *safe;
    
    list_for_each_entry_safe (cur, safe, head, list) {
        free(cur);
    }
    free(head);
}


struct list_head *list_tail(struct list_head *head)
{
    struct list_head *tail = head -> prev;
    return tail;
}


int list_length(struct list_head *head)
{

    if(!head)
        return 0;
    int n = 0;
    struct list_head *cur = head -> next;
    while(cur != head){
        n++;
        cur = cur -> next;
    }
    return n;
}

struct list_head *list_new()
{
    
    struct list_head *head = malloc(sizeof(struct list_head)); 
    if(!head)
        return NULL;
        
    head -> next = head;
    head -> prev = head;
    return head;
}

void quick_sort(struct list_head *list)
{
    int n = list_length(list);
    int pivot_value;
    int i = 0;
    int max_level = 2 * n;

    struct list_head *begin[max_level], *end[max_level];
    struct list_head *result = list_new(), *left = list_new(), *right = list_new();
    
    begin[0] = list;
            
    while (i >= 0) {
        struct list_head *L = begin[i];

        if (L ->next !=list_tail(L)) {
            struct list_head *pivot = list_new();
            struct list_head *cur =  L ->next;
            
            int max = list_length(L) -1;
            int min = 0;
            int cnt = rand() % (max - min + 1) + min;

            struct list_head *p = cur;
            while(cnt){
                p = p->next;
                cnt--;
            }
            if(cur == p)
                cur = cur->next;
            
            pivot_value = list_entry(p,node_t,list)->value;
            list_move_tail(p, pivot);

            while (!list_empty(L)) {   
                cur = cur -> next;
                list_move_tail(cur -> prev, list_entry(cur -> prev,node_t,list)->value > pivot_value ? right : left);
                
            }

            list_splice_tail_init(left,begin[i]);
            begin[i+1] = list_new();
            begin[i+2] = list_new();
            list_splice_tail_init(pivot,begin[i+1]);
            list_splice_tail_init(right,begin[i+2]);

            i += 2;
        } else {
            if (!list_empty(L))
                list_splice_tail_init(L, result);
            i--;
        }
    }
    list_splice_tail_init(result,list);
}

int main(int argc, char **argv)
{
    struct list_head *head = malloc(sizeof(struct list_head));
    head -> next = head;
    head -> prev = head;

    int count = 6;

    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; i++){
        test_arr[i] = i;
    }

    shuffle(test_arr, count);
    
    while (count--){
        list_construct(head, test_arr[count]);
    }

    int size = list_length(head);

    quick_sort(head);
    assert(list_is_ordered(head));

    struct list_head *tmp =head;
    
    list_for_each(tmp,head){
        printf("\n %d", list_entry(tmp,node_t,list)->value);
    }
    return 0;
}



