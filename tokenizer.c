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

// Arrays for characters in each token
char initialSymbol[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', \
'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', \
'y', 'z', 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P', \
'Q','R','T','U','V','W','X','Y','Z', '!', '$', '%', '&', '*', '/', ':', '<', \
'=', '>', '?', '~', '|', '_', '^'};
char subsequent[] = {'.', '+', '-'};
char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

bool isPart(char c, char array[], int length) {

    for(int i = 0; i < length; i++) {
        if(c == array[i]) {
            return true;
        }
    }
    // Not in token
    return false;
}
// Read all of the input from stdin, and return a linked list consisting of the
// tokens. Types boolean, integer, double, string, symbol, open, close
Value *tokenize() {
    
    char charRead;
    Value *list = makeNull();
    charRead = (char)fgetc(stdin);

    while (charRead != EOF) {

        // Ignore Comments
        if (charRead == ';') {
            while(charRead != '\n') {
                charRead = (char)fgetc(stdin);
            }

        } else if (charRead != '\n' && charRead != '\t' && charRead != ' ') {
            // New Token
            Value *newToken = talloc(sizeof(Value));

            // OPEN
            if (charRead == '(')  {
                newToken -> type = OPEN_TYPE;
                char *open = "(";
                newToken -> s = talloc(sizeof(char));
                newToken -> s = open;

            } 
            // CLOSE
            else if (charRead == ')'){
                newToken -> type = CLOSE_TYPE;
                char *close = ")";
                newToken -> s = talloc(sizeof(char));
                newToken -> s = close;
            }

            // BOOLEAN
            else if (charRead == '#') {
                charRead = (char)fgetc(stdin);
                newToken -> type = BOOL_TYPE;

                if(charRead == 'f') {
                    newToken -> i = 0;
                } else if (charRead == 't') {
                    newToken -> i = 1;
                } else {
                    // Invalid Boolean
                    printf("Syntax error\n");
                    texit(1);
                }
            // PLUS (SYMBOL)
            } else if (charRead == '+') {

                newToken -> type = SYMBOL_TYPE;
                char *sym = "+";
                newToken -> s = talloc(sizeof(char));
                newToken -> s = sym;

            // MINUS / INTEGER / DOUBLE
            } else if ((charRead == '-') || (isPart(charRead, digits, 10))){

                bool hasNeg = false;
                char charLast;

                if (charRead == '-') {
                    hasNeg = true;
                    charLast = charRead;
                    charRead = (char)fgetc(stdin);
                }                
                if (isPart(charRead, digits, 10)) {
        
                    bool hasPeriod = false;
                    int numSize = 0;
                    char numberStr[300];
                    strcpy(numberStr, "");
                    if(hasNeg) { 
                        strncat(numberStr, &charLast, 1);
                    }
                    while(charRead != EOF && 
                    (isPart(charRead, digits, 10) ||
                    charRead == '.'))
                    {
                        if(charRead == '.') {
                            if(hasPeriod) {
                                printf("Syntax error\n");
                                texit(1);
                            }
                            else {
                                hasPeriod = true;
                            }
                        }
                        strncat(numberStr, &charRead, 1);
                        charRead = (char)fgetc(stdin);
                    }

                    ungetc(charRead, stdin);

                    char *convertPtr;
                    long intNumber;
                    double doubNumber;

                    if(hasPeriod) {
                        doubNumber = strtod(numberStr, &convertPtr);
                        newToken -> type = DOUBLE_TYPE;
                        newToken -> d = doubNumber;
                    } else {
                        intNumber = strtol(numberStr, &convertPtr, 10);
                        newToken -> type = INT_TYPE;
                        newToken -> i = intNumber;
                    }
                }
                else {
                    ungetc(charRead, stdin);
                    newToken -> type = SYMBOL_TYPE;
                    char *sym = "-";
                    newToken -> s = talloc(sizeof(char));
                    newToken -> s = sym;
                }  
            }
            // SYMBOLS
            else if(isPart(charRead, initialSymbol, 67)) {
                
                int symSize = 0;
                char symbol[300];
                strcpy(symbol, "");

                while(charRead != EOF &&
                (isPart(charRead, initialSymbol, 67) ||
                isPart(charRead, digits, 10) ||
                isPart(charRead, subsequent, 10)))
                {
                    strncat(symbol, &charRead, 1);
                    charRead = (char)fgetc(stdin);
                }

                ungetc(charRead, stdin);

                newToken -> type = SYMBOL_TYPE;
                newToken -> s = talloc(sizeof(char) * (strlen(symbol) + 1));
                strcpy(newToken -> s, symbol);

            //STRINGS
            } else if(charRead == '"') {

                int strSize = 0;
                char string[300];
                strcpy(string, "");

                strncat(string, &charRead, 1);
                charRead = (char)fgetc(stdin);

                while(charRead != '"') {
                    strncat(string, &charRead, 1);
                    charRead = (char)fgetc(stdin);
                }

                strncat(string, &charRead, 1);

                newToken -> type = STR_TYPE;
                newToken -> s = talloc(sizeof(char) * (strlen(string) + 1));
                strcpy(newToken -> s, string);

            }
            else {
                printf("Syntax error: Unrecoginzed Character '%c'\n", charRead);
                texit(1);
            }

            // Adding to list
            list = cons(newToken, list);

        }
        // Iterature through file
        charRead = (char)fgetc(stdin);
 
    }

    fclose(stdin);

    // Reverse List
    Value *revList = reverse(list);
    return revList;

}



// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list) {

    Value *cur = list;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);

        switch (cur -> type) {
            case BOOL_TYPE:
                if (cur -> i == 1) {
                    printf("#t:boolean\n");
                }
                else {
                    printf("#f:boolean\n");
                }
                break;
            case INT_TYPE:
                printf("%i:integer\n", cur -> i);
                break;
            case DOUBLE_TYPE:
                printf("%f:double\n", cur -> d);        
                break;
            case STR_TYPE:
                printf("%s:string\n", cur -> s);
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", cur -> s);
                break;
            case OPEN_TYPE:
                printf("%s:open\n", cur -> s);
                break;
            case CLOSE_TYPE:
                printf("%s:close\n", cur -> s);
                break;
            default:
                printf("ERROR: Unexpected Type\n");
                break;
        }
        cur = follow;
    }

}