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

/* 
Shaun Baron-Furuyama and James Brink
CS 251 -- linkedlist for Scheme interpreter
*/

// Create a new NULL_TYPE value node.
Value *makeNull() {

    Value *nullNode = talloc(sizeof(Value));
    nullNode -> type = NULL_TYPE;

    return nullNode;

}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr) {

    Value *consNode = talloc(sizeof(Value));
    consNode -> type = CONS_TYPE;
    consNode -> c.car = newCar;
    consNode -> c.cdr = newCdr;

    return consNode;

}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list) {

    Value *cur = list;

    printf("<<LIST>>\n");

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);

        switch (cur -> type) {
            case INT_TYPE:
                printf("%i\n", cur -> i);
                break;
            case DOUBLE_TYPE:
                printf("%.2f\n", cur -> d);
                break;
            case STR_TYPE:
                printf("%s\n", cur -> s);        
                break;
            case PTR_TYPE:
                printf("%p\n", cur -> p);
                break;
            case SYMBOL_TYPE:
                printf("%s\n", cur -> s);        
                break;
            case BOOL_TYPE:
                if (cur -> i == 1) {
                    printf("#t\n");
                }
                else {
                    printf("#f\n");
                }
                break;
            default:
                printf("Unexpected type: Null Value?\n");
                break;
        }
        cur = follow;
    }
    printf("<<NULL>>\n");
}

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.

Value *reverse(Value *list) {

    Value *reversedList = makeNull();
    Value *cur = list;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        
        reversedList = cons(car(cur), reversedList);
        cur = follow;
    
    }

    return reversedList;
}

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list) {

    assert(list -> type == CONS_TYPE);
    return list -> c.car;

}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {

    assert(list -> type == CONS_TYPE);
    return list -> c.cdr;

}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value) {

    if (value -> type == NULL_TYPE) {
        return true;
    }

    return false;

}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value) {

    int length = 0;

    while (!isNull(value)) {
        length++;
        value = cdr(value);
    }

    return length;
}