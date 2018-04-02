//
// os_list.c
//
// This file contains a very simple and dumb singly-Linked-List for storing pointers.
// This is NOT how you should build a list.
//
// Feel free to read, modify or improve this, althrough you 
// really dont't have to touch it for the assignment.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>

// Basic node type contained in list, which carries a void ptr.
typedef struct node_t {
    void* data;
    struct node_t* next;
} node_t;

// Basic linked list built up of nodes. Head, tail and length are stored.
typedef struct list_t {
    unsigned int length;
    node_t* head;
    node_t* tail;
} list_t;

/**
 * Allocates an empty list.
 * Returns pointer to list.
 */
list_t* create_list() {
    list_t* list = malloc(sizeof(list_t));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

/**
 * Deallocates a list, all its nodes, and the data inside those nodes.
 * - list: the list to destroy.
 */
void destroy_list(list_t* list) {
    node_t* del_node = list->head;
    if (del_node == NULL) {
        return;
    }

    node_t* node = del_node->next;
    free(del_node);
    while (node != NULL) {
        del_node = node;
        node = node->next;
        free(del_node);
    }
    free(list);
}

/**
 * Push pointer to data onto head of list
 * - list: list to add to.
 * - data: data pointer to add.
 */
void push_front_p(list_t* list, void* data) {
    // Create new node
    node_t* node = malloc(sizeof(node_t));
    node->data = data;

    // Push onto list
    if (list->tail == NULL) {
        list->tail = node;
    }
    node->next = list->head;
    list->head = node;

    list->length += 1;
}

/**
 * Pop pointer to data from head of list.
 * - list: list to pop from
 * Returns NULL if empty, valid (?) pointer otherwise.
 */
void* pop_front_p(list_t* list) {
    node_t* node = list->head;
    if (node != NULL) {
        // Update list
        list->length -= 1;
        if (list->length == 0) {
            list->head = NULL;
            list->tail = NULL;
        } else if (list->length == 1) {
            list->tail = list->head;
            list->head = list->head->next;
        } else {
            list->head = list->head->next;
        }
        
        // Return data pointer, destroy node
        void* data = node->data;
        free(node);
        return data;
    } else {
        return NULL;
    }
}

/**
 * Push pointer to data onto tail of list.
 * - list: list to add to.
 * - data: data pointer to add.
 */
void push_back_p(list_t* list, void* data) {
    // Create new node
    node_t *node = malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;

    // Push onto list
    if (list->tail == NULL) {
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;

    list->length += 1;
}

/**
 * Pop pointer to data from tail of list.
 * - list: list to pop from.
 * Returns NULL if empty, valid (?) pointer otherwise.
 */
void* pop_back_p(list_t* list) {
    node_t* node = list->tail;
    if (node != NULL) {
        list->length -= 1;
        if (list->length == 0) {
            list->head = NULL;
            list->tail = NULL;
        } else if (list->length == 1) {
            list->tail = list->head;
            list->head->next = NULL;
        } else {
            // Should have made a DLL, but oh well
            node_t* next = list->head;
            while (next->next != list->tail) {
                next = next->next;
            }
            list->tail = next;
            next->next = NULL;
        }
        
        // Return data pointer, destroy node
        void* data = node->data;
        free(node);
        return data;
    } else {
        return NULL;
    }
}

/**
 * Get pointer to node-link at index i of list.
 * Should not be used by any external access to the list!
 * - list: list to access.
 * - i: index of stored data.
 * Returns NULL if out of bounds, or valid (?) pointer otherwise.
 */
node_t* get_node_p(list_t *list, unsigned int i) {
    node_t* node = list->head;
    unsigned int j = 0;
    while (j++ < i) {
        if (node == NULL) {
            return NULL;
        }
        node = node->next;
    }
    return node;
}

/**
 * Remove data from list at index i.
 * Warning: the node is destroyed, but this implies the data is no
 * longer attached to the list. If the list is destroyed, the data is not!
 * - list: list to access
 * - i: index of stored data
 * Returns NULL if out of bounds, or valid (?) pointer to removed data.
 */
void* remove_p(list_t* list, unsigned int i) {
    // Remove head or tail
    if (i <= 0) {
        return pop_front_p(list);
    } else if (i >= list->length - 1) {
        return pop_back_p(list);
    }

    // Get the dead node
    node_t* node = get_node_p(list, i);
    void* data = node->data;

    // Update list
    node_t* prev = get_node_p(list, i - 1);
    node_t* next = get_node_p(list, i + 1);
    prev->next = next;
    list->length -= 1;

    free(node);
    return data;
}

/**
 * Get pointer to stored data at index i.
 * - list: list to access.
 * - i: index of stored data.
 * Returns NULL if out of bounds, or valid (?) pointer otherwise.
 */
void* get_p(list_t* list, unsigned int i) {
    node_t* node = get_node_p(list, i);
    if (node == NULL) {
        return NULL;
    }
    return node->data;
}