#include <stdint.h>
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
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *itr, *safe;
    list_for_each_entry_safe (itr, safe, l, list) {
        q_release_element(itr);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del(&ele->list);
    if (sp) {
        size_t n = strlen(ele->value) + 1;
        n = bufsize < n ? bufsize : n;
        strncpy(sp, ele->value, n);
        sp[n - 1] = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del(&ele->list);
    if (sp) {
        size_t n = strlen(ele->value) + 1;
        n = bufsize < n ? bufsize : n;
        strncpy(sp, ele->value, n);
        sp[n - 1] = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    size_t n = 0;
    struct list_head *itr;
    list_for_each (itr, head)
        ++n;
    return n;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *ahead = head->next, *back = head->prev;
    while (ahead != back && ahead != back->prev) {
        ahead = ahead->next;
        back = back->prev;
    }
    list_del(ahead);
    element_t *ele = list_entry(ahead, element_t, list);
    q_release_element(ele);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    element_t *itr, *safe;
    bool duplicate = false;
    list_for_each_entry_safe (itr, safe, head, list) {
        if (&safe->list != head && strcmp(itr->value, safe->value) == 0) {
            duplicate = true;
            list_del(&itr->list);
            q_release_element(itr);
        } else if (duplicate) {
            list_del(&itr->list);
            q_release_element(itr);
            duplicate = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_is_singular(head))
        return;
    struct list_head *itr;
    list_for_each (itr, head) {
        if (itr->next != head) {
            list_del(itr);
            list_add(itr, itr->next);
        } else
            return;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *itr, *safe, *last = head->prev;
    list_for_each_safe (itr, safe, head) {
        if (itr != last)
            list_move(itr, last);
        else
            return;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_is_singular(head) || k == 1)
        return;
    struct list_head *k_last = head, *k_lastr = head->next;
    for (int i = 0; i < k; ++i) {
        k_last = k_last->next;
        if (head == k_last)
            return;
    }
    struct list_head *itr, *safe;
    list_for_each_safe (itr, safe, head) {
        if (itr != k_last)
            list_move(itr, k_last);
        else {
            k_last = k_lastr;
            for (int i = 0; i < k; ++i) {
                k_last = k_last->next;
                if (head == k_last)
                    return;
            }
            safe = k_lastr = k_lastr->next;
        }
    }
}


static struct list_head *split(struct list_head *head)
{
    struct list_head *fast = head, *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct list_head *b = slow->next;
    slow->next = NULL;
    return b;
}


static struct list_head *merge(struct list_head *a, struct list_head *b)
{
    // ptr : Update list_head->next to link node
    // node : Each comparison will advance one either a or b
    struct list_head *head = NULL, **ptr = &head, **node;
    for (; a && b; *node = (*node)->next) {
        node = strcmp(list_entry(a, element_t, list)->value,
                      list_entry(b, element_t, list)->value) <= 0
                   ? &a
                   : &b;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) a | (uintptr_t) b);
    return head;
}

static struct list_head *MergeSort(struct list_head *a)
{
    struct list_head *b = split(a);
    if (a->next)
        a = MergeSort(a);
    if (b->next)
        b = MergeSort(b);
    return merge(a, b);
}
__attribute__((unused)) static void mergeSort_iterative_buttomup(
    struct list_head *head)
{
    // first merge 1:1
    struct list_head *a = head->next, *b = NULL, *next_a, *prev_head = head;
    while (a && a->next) {
        b = a->next;
        next_a = b->next;
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            a->next = b;
            prev_head->prev = a;
            prev_head = a;
            b->prev = NULL;
            b->next = NULL;
        } else {
            b->next = a;
            prev_head->prev = b;
            prev_head = b;
            a->prev = NULL;
            a->next = NULL;
        }
        a = next_a;
    }
    if (a) {
        prev_head->prev = a;
        prev_head = a;
    }
    prev_head->prev = NULL;

    while (head->prev->prev) {  // check merge block chain is one
        a = head->prev;
        prev_head = head;
        while (a && a->prev) {
            b = a->prev;
            next_a = b->prev;
            a->prev = NULL;
            b->prev = NULL;

            // merge
            struct list_head *merge_head = NULL, **ptr = &merge_head, **node;
            for (; a && b; *node = (*node)->next) {
                node = strcmp(list_entry(a, element_t, list)->value,
                              list_entry(b, element_t, list)->value) <= 0
                           ? &a
                           : &b;
                *ptr = *node;
                ptr = &(*ptr)->next;
            }
            *ptr = (struct list_head *) ((uintptr_t) a | (uintptr_t) b);
            prev_head->prev = merge_head;
            prev_head = merge_head;
            a = next_a;
        }
        if (a)
            prev_head->prev = a;
    }
    head->next = head->prev;
}


static void QuickSort(struct list_head *head)
{
    if (!head || list_empty(head) || head->next == head->prev)
        return;
    struct list_head less, greater, middle;
    INIT_LIST_HEAD(&less);
    INIT_LIST_HEAD(&greater);
    INIT_LIST_HEAD(&middle);
    struct list_head *pivot = head->next;
    char *pivot_val = list_entry(pivot, element_t, list)->value;
    list_del(pivot);
    struct list_head *itr, *safe;
    bool less_change = true, greater_change = true;
    list_for_each_safe (itr, safe, head) {
        int result = strcmp(list_entry(itr, element_t, list)->value, pivot_val);
        if (result < 0) {
            if (less_change)
                list_add(itr, &less);
            else
                list_add_tail(itr, &less);
            less_change = !less_change;
        } else if (result > 0) {
            if (greater_change)
                list_add(itr, &greater);
            else
                list_add_tail(itr, &greater);
            greater_change = !greater_change;
        } else {
            list_add(itr, &middle);
        }
    }
    QuickSort(&less);
    QuickSort(&greater);
    INIT_LIST_HEAD(head);
    list_add(pivot, head);
    list_splice(&middle, head);
    list_splice(&less, head);
    list_splice_tail(&greater, head);
}

int sort_option = 1;
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || head->next == head->prev)
        return;
    if (sort_option) {  // merge sort
        head->prev->next = NULL;
        head->next = MergeSort(head->next);
        // fix prev pointer
        struct list_head *prev, *cur = head;
        while (cur->next) {
            prev = cur;
            cur = cur->next;
            cur->prev = prev;
        }
        head->prev = cur;
        cur->next = head;
    } else {  // quick sort
        QuickSort(head);
    }
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int count = 1;
    struct list_head *itr = head->prev, *safe;
    char *cur_max = list_entry(itr, element_t, list)->value;
    for (itr = itr->prev, safe = itr->prev; itr != head;
         itr = safe, safe = itr->prev) {
        char *tmp = list_entry(itr, element_t, list)->value;
        if (strcmp(tmp, cur_max) >= 0) {
            cur_max = tmp;
            ++count;
        } else {
            list_del(itr);
            q_release_element(list_entry(itr, element_t, list));
        }
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;
    struct list_head *q_merge =
        list_entry(head->next, queue_contex_t, chain)->q;
    for (struct list_head *c_itr = head->next->next; c_itr != head;
         c_itr = c_itr->next) {
        struct list_head *q_itr, *safe,
            *q_other = list_entry(c_itr, queue_contex_t, chain)->q,
            *node_merge = q_merge->next;
        list_for_each_safe (q_itr, safe, q_other) {
            while (node_merge != q_merge) {
                if (strcmp(list_entry(q_itr, element_t, list)->value,
                           list_entry(node_merge, element_t, list)->value) <=
                    0) {
                    break;
                } else
                    node_merge = node_merge->next;
            }
            list_add_tail(q_itr, node_merge);
        }
        INIT_LIST_HEAD(q_other);
    }
    return q_size(q_merge);
}
