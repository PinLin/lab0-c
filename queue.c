#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
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

    element_t *curr, *next;
    list_for_each_entry_safe (curr, next, l, list)
        q_release_element(curr);
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add(&node->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add_tail(&node->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp) {
        // If sp is non-NULL, copy the value of removed element to *sp.
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp) {
        // If sp is non-NULL, copy the value of removed element to *sp.
        strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    bool is_dup = false;
    element_t *curr, *next;
    list_for_each_entry_safe (curr, next, head, list) {
        if (curr->list.next != head && strcmp(curr->value, next->value) == 0) {
            list_del(&curr->list);
            q_release_element(curr);
            is_dup = true;
        } else if (is_dup) {
            list_del(&curr->list);
            q_release_element(curr);
            is_dup = false;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *curr, *next, *temp = NULL;
    list_for_each_safe (curr, next, head) {
        if (!temp) {
            list_del(curr);
            temp = curr;
        } else {
            list_add(temp, curr);
            temp = NULL;
        }
    }
    if (temp)
        list_add_tail(temp, head);
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *curr, *next;
    list_for_each_safe (curr, next, head) {
        list_move(curr, head);
    }
}

struct list_head *q_merge_sort(struct list_head *head);

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    // Unlink list head and list tail
    head->prev->next = NULL;
    head->next = q_merge_sort(head->next);
    struct list_head *curr = head;

    // Recover circular doubly linked list
    while (curr->next) {
        curr->next->prev = curr;
        curr = curr->next;
    }
    head->prev = curr;
    curr->next = head;
}

struct list_head *q_merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    // Divide
    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    slow->prev->next = NULL;

    struct list_head *left = q_merge_sort(head);
    struct list_head *right = q_merge_sort(slow);

    // Conquer
    struct list_head *merged = NULL, **next_ptr = &merged;
    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            *next_ptr = left;
            left = left->next;
            next_ptr = &(*next_ptr)->next;
        } else {
            *next_ptr = right;
            right = right->next;
            next_ptr = &(*next_ptr)->next;
        }
    }
    *next_ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);
    return merged;
}

/*
 * Attempt to shuffle every node.
 */
void q_shuffle(struct list_head *head)
{
    if (!head)
        return;

    srand(time(NULL));

    struct list_head *node1 = head->prev;
    int len = q_size(head);
    while (len > 1) {
        struct list_head *node2 = head->next;
        for (int i = rand() % len; i > 0; i--) {
            node2 = node2->next;
        }

        char *temp = list_entry(node1, element_t, list)->value;
        list_entry(node1, element_t, list)->value =
            list_entry(node2, element_t, list)->value;
        list_entry(node2, element_t, list)->value = temp;

        node1 = node1->prev;
        len--;
    }
}
