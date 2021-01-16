/* 
 * Generic linked Stack Library v1.1
 * 
 * @author Gabriel-AB
 * https://github.com/Gabriel-AB
 * 
 */
#pragma once
#include <stdlib.h>

struct stack_node {
    struct stack_node *next;
    char data[];
};

typedef struct stack {
    struct stack_node *last;
    const size_t data_size;
    size_t size;
} *Stack;

// look the macros
void _stack_push(Stack stack, void *data);
void* _stack_pop(Stack stack);
Stack _stack_create(size_t data_size, size_t size, void *data);

// Free a created stack
void StackDelete(Stack stack);

/* 
 * ### Push a new item into the stack
 * obs: the literal type of data is checked, so put it right
 * 
 * ex: stack of double
 * > StackPush(s, 10) // wrong
 * > StackPush(s, 10.2) // right
 * 
 * ex: stack of float
 * > StackPush(s, 2.) // wrong
 * > StackPush(s, 2.f) // right
 * > StackPush(s, 3.14f) // right
 */
#define StackPush(stack, data) _stack_push(stack, (__typeof__(data)[]){data})

/* 
 * ### Pop the last item from the stack
 * `type` is the type of data you expect to receive
 */
#define StackPop(stack, type) *(type*)_stack_pop(stack)

/*
 * ### Creates a new Stack using a array literal
 * 
 * ex:
 *   StackCreate(float, {})
 *   StackCreate(int, {1,2,3})
 */
#define StackCreate(type, array...) ({\
    type arr[] = array;\
    _stack_create(sizeof(type), sizeof(arr)/sizeof(type), arr);\
})