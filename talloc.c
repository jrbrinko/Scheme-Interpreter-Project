#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include "tokenizer.h"


// Global linked list
Value *memoryList;
bool tallocInit = false;

Value *initializeMemoryList() {
    
    Value *nullNode = malloc(sizeof(Value));
    nullNode -> type = NULL_TYPE;

    return nullNode;
}
// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size) {

    if(tallocInit == false) {
        memoryList = initializeMemoryList();
        tallocInit = true;
    }

    Value *newMemory = malloc(sizeof(Value));
    newMemory -> type = PTR_TYPE;
    newMemory -> p = malloc(size);
    //memoryList = cons(newMemory, memoryList);

    // Add to list
    Value *consNode = malloc(sizeof(Value));
    consNode -> type = CONS_TYPE;
    consNode -> c.car = newMemory;
    consNode -> c.cdr = memoryList;
    memoryList = consNode;

    return newMemory -> p;
}


// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree() {

    //display(memoryList);

    Value *cur = memoryList;
    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        
        // Free pointer
        free(car(cur) -> p);

        // Free Value
        free(car(cur));

        // Free List's Cons Cell
        free(cur);

        // Iterate through list
        cur = follow;

    }

    //Free NULL (the head)
    free(cur);
    tallocInit = false;
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {

    tfree();
    exit(status);
}


