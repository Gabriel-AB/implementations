/* 
 * Generic double linked List Library v1.4
 * 
 * @author Gabriel-AB
 * https://github.com/Gabriel-AB
 * 
 */

#include "list.h"
#include <stdlib.h>
#include <string.h>

#ifdef LIST_DEBUG
# include <stdio.h>
#endif


// ====================== LIBRARY INTERNAL FUNCTIONS ====================== //

/* 
 * Allocate a list_node and return the pointer
 * obs: size in bytes
 */
static struct list_node *_list_new_node(size_t size) {
    struct list_node *ptr = calloc(1,sizeof(struct list_node) + size);
#ifdef LIST_DEBUG
    if (!ptr) perror("LIST_DEBUG: Failed to create node");
#endif
    return ptr;
}

/* 
 * Free the given node conecting neighbors to each other
 * and return the allocated data pointer
 */
static void _list_detach_node(struct list_node *n) {
    
}

/* 
 * Find the linked node by the given index.
 */
static struct list_node * _list_find(struct list_node * node, int index) {
    if (index < 0) {
        while (++index && node)
            node = node->back;
    } else {
        while (index-- && node)
            node = node->next;
    }
    return node;
}

/* 
 * List constrained index finder 
 * if index is negative, searchs in reverse. 
 * return: the node
 */
static struct list_node * _list_node_at(List list, int index) {
#ifdef LIST_DEBUG
    if (list->size < (size_t)(index < 0 ? -index-1 : index))
        perror("LIST_DEBUG: Index out of the list!");
#endif
    return _list_find((index < 0) ? list->tail : list->head, index);
}

// =========================== PUBLIC FUNCTIONS =========================== //

void * _list_at(AnyList list, int index) {
    return _list_node_at(list, index)->data;
}

// ### Constructor
void* _list_create(size_t data_size, size_t initial_size, void * initial_values) {
    List list = calloc(1,sizeof(struct list));
    *(size_t*)&list->internal.dsize = data_size;
    if (initial_values)
        _list_pushback(list, initial_size, initial_values);
    else if (initial_size)
        list_resize(list, initial_size);
    return list;
}



// Push value in list's end.
void _list_pushback(AnyList list, size_t num_elements, void *data) {
    List l = list;
    while (num_elements--) {
        struct list_node *new_node = _list_new_node(l->internal.dsize);

        if (data) {
            memcpy(new_node->data, data, l->internal.dsize);
            data = (char*)data + l->internal.dsize;
        }

        if (l->size++ > 0) {
            l->tail->next = new_node;
            new_node->back = l->tail;
            l->tail = new_node;
        }
        else l->head = l->tail = new_node;
    }
}

// Push value in list's begin.
void _list_pushfront(AnyList list, size_t num_elements, void * data) {
    List _list = list;
    while (num_elements--) {
        struct list_node *new_node = _list_new_node(_list->internal.dsize);

        if (data) {
            void* curr = (char*)data + num_elements*_list->internal.dsize;
            memcpy(new_node->data, curr, _list->internal.dsize);
        }

        if (_list->size++ > 0) {
            _list->head->back = new_node;
            new_node->next = _list->head;
            _list->head = new_node;
        }
        else _list->head = _list->tail = new_node;
    }
}

//Push value in the index especified
void _list_push(AnyList _list, int index, void * item) {
    List list = _list;
    struct list_node *old_node = _list_node_at(list, index);
    if (old_node == NULL) return;

    struct list_node *new_node = _list_new_node(list->internal.dsize);
    if (new_node == NULL) return;

    // Goes before old item
    if (index >= 0) {
        new_node->next = old_node;
        new_node->back = old_node->back;
        if (old_node == list->head)
            list->head = new_node;
        else
            old_node->back->next = new_node;
        old_node->back = new_node;

    // Goes after old item
    } else {
        new_node->back = old_node;
        new_node->next = old_node->next;
        if (old_node == list->tail)
            list->tail = new_node;
        else
            old_node->next->back = new_node;
        old_node->next = new_node;
    }
    if (item) memcpy(new_node->data, item, list->internal.dsize);
    list->size++;

#ifdef LIST_DEBUG
    else perror("LIST_DEBUG: Index not found!");
#endif

}

// Retrieve the in the index specified
void *_list_pop_node(AnyList _list, AnyListNode _node) {
    List list = _list;
    struct list_node* node = _node;

    if (node == list->tail)
        list->tail = node->back;
    if (node == list->head)
        list->head = node->next;
    list->size--;

    if (list->internal.pop)
        free(list->internal.pop);

    if (node->back) node->back->next = node->next;
    if (node->next) node->next->back = node->back;
    list->internal.pop = node;
    
    return node->data;
}

void* _list_pop(AnyList list, int index) {
    _list_pop_node(list, _list_node_at(list, index));
}

// Resize a list allocating new memory,
void list_resize(AnyList list, unsigned int new_size) {
    List _list = list;
    if (_list->size < new_size) {
        int count = new_size -_list->size;
        _list_pushback(_list, count, NULL);
    } else {
        int count = _list->size -new_size;
        for (int i = 0; i < count; i++)
            _list_pop(_list, -1);
    }
}

// Returns a copy of `src`
AnyList list_copy(AnyList src) {
    List list = src;
    List dst = _list_create(list->internal.dsize, 0, 0);
    struct list_node* node = list->head;
    while (node) {
        _list_pushback(dst, 1, node->data);
        node = node->next;
    }
    return dst;
}

void list_clear(AnyList list) {
    List _list = list;
    while (_list->size > 0)
        _list_pop(list, -1);
    free(_list->internal.pop);
    _list->internal.pop = NULL;
}


// convert to a array
void list_to_array(AnyList list, void* result) {
    List _list = list;
    struct list_node *n = _list->head;
    for (size_t i = 0; i < _list->size; i++) {
        void * array_element = (char*)result + i*_list->internal.dsize;
        memcpy(array_element, n->data, _list->internal.dsize);
        n = n->next;
    }
}

void list_delete(AnyList list) {
    list_clear(list);
    free(list);
}

void* list_sublist(AnyList list, unsigned int begin, unsigned int end) {
    List _list = list;
    List result = _list_create(_list->internal.dsize, 0, 0);
    struct list_node* node = _list->head;

    end -= begin;
    while (begin--)
        node = node->next;

    while (end-- && node) {
        _list_pushback(result, 1, node->data);
        node = node->next;
    }
    return result;
}