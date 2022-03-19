#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"
#include "parser.h"


// Takes a status number to print a syntax error
// then exits the program by calling texit()
void syntaxError(int status) {
  if (status == 0) {
    printf("Syntax error: not enough close parentheses\n");
  } else if (status == 1) {
    printf("Syntax error: too many close parentheses\n");
  } else {
    printf("Syntax error\n");
  }
  texit(1);
}


// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {
  Value *tree = makeNull();
  Value *stack = makeNull();
  int parenCount = 0; //Keeps count of the balance 
                      //between open and close parentheses

  Value *current = tokens;
  assert(current != NULL && "Error (parse): null pointer");

  while (current->type != NULL_TYPE) {
    
    Value *token = car(current); //One individual token
    
    //If it's open_type, add token to stack and increment parenCount
    if (token -> type == OPEN_TYPE) {
      parenCount++;
      stack = cons(token, stack);
    //The case where token is CLOSE_TYPE
    //Makes a subtree of all tokens in the stack
    //until open_type is encountered
    } else if (token -> type == CLOSE_TYPE) {
      if (parenCount == 0) {
        syntaxError(1);
      }
      parenCount--;
      Value *subTree = makeNull();

      //Iterates through stack until open paren is found
      while (car(stack) -> type != OPEN_TYPE) {
        if (stack -> type == NULL_TYPE) {
          syntaxError(1);
        }
        subTree = cons(car(stack), subTree);
        stack = cdr(stack);
      }
      stack = cdr(stack); //increment stack down a level to pass open paren
      stack = cons(subTree, stack);
    } else {
      stack = cons(token, stack);
    }
    
    current = cdr(current);

    if (parenCount == 0) {
      tree = cons(car(stack), tree);
      stack = cdr(stack);

    }
  }

  //If there are too many open parens
  if (parenCount > 0) {
    syntaxError(0);
  //If there are too many close parens
  } else if (parenCount < 0) {
    syntaxError(1);
  }

  return reverse(tree);
}


// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
  while(!isNull(tree)) {
    Value *current = car(tree);
    //If cell is cons_type, drop down a level and
    // call print tree recursively on lower level
    if (current -> type == CONS_TYPE) {
      printf("(");
      printTree(current);
      printf(") ");
    //Otherwise, just print the object in the tree
    } else {
      if (current -> type == BOOL_TYPE) {
        if (current -> i == 1) {
          printf("#t ");
        } else {
            printf("#f ");
        }
      } else if (current -> type == INT_TYPE) {
        printf("%i ", current -> i);
      } else if (current -> type == DOUBLE_TYPE) {
        printf("%f ", current -> d);
      } else if (current -> type == NULL_TYPE) {
        printf("()\n");
      } else {
        printf("%s ", current -> s);
      }
    }
    tree = cdr(tree);
  }
}


