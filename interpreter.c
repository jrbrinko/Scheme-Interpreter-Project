/*
Shaun Baron-Furuyama
James Brink
CS 251
Scheme Interpreter Project
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

// ================================================
// ================= ERROR ===================
// ================================================

void evaluationError(char *err) {

    printf("Evaluation error: %s\n", err);
    texit(1);

}

// ================================================
// ================= PRIMITIVES ===================
// ================================================
Value *primitiveAdd(Value *args) {

    bool hasDouble = false;

    double sum = 0;
    Value *cur = args;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);
        if(cur -> type == DOUBLE_TYPE) {
            hasDouble = true;
            sum += cur -> d;
        }else if(cur -> type == INT_TYPE)  {
            sum += cur -> i;
        } else {
            // Trying to add non-numeric type
            evaluationError("invalid type for +");
        }
        cur = follow;
    }

    Value *result = talloc(sizeof(Value));
    if(hasDouble) {
        result -> type = DOUBLE_TYPE;
        result -> d = sum;
    } else {
        result -> type = INT_TYPE;
        // since sum is a double cast to int
        result -> i = (int) sum;
    }   

    return result;

}

Value *primitiveNull(Value *args) {

    // Error checking
    if(length(args) != 1) {
        evaluationError("null? only takes one parameter.");
    }

    // Create new Bool Value
    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;
    result -> i = 0;


    // Null case
    if(car(args) -> type == NULL_TYPE) {
        result -> i = 1;
    }

    return result;
}

Value *primitiveCar(Value *args) {

    // Error checking
    if(length(args) != 1) {
        evaluationError("car() only takes one parameter.");
    }

    if(car(args) -> type != CONS_TYPE) {
        evaluationError("Wrong type of argument in car, expecting list");
    }

    return car(car(args));

}

Value *primitiveCdr(Value *args) {

    // Error checking
    if(length(args) != 1) {
        evaluationError("cdr() only takes one parameter.");
    } else if(car(args) -> type != CONS_TYPE) {
        evaluationError("Wrong type of argument in cdr, expecting list");
    }

    return cdr(car(args));
}


Value *primitiveCons(Value *args) {

    // Error Checking
    if(length(args) != 2) {
        evaluationError("cons() only takes two parameters.");
    }

    Value *first = car(args);
    Value *second = car(cdr(args));

    // Create new cons cells where 
    // car --> first
    // cdr --> second
    Value *newCons = talloc(sizeof(Value));
    newCons -> type = CONS_TYPE;
    newCons -> c.car = first;
    newCons -> c.cdr = second;


    return newCons;
}

Value *primitiveSubtraction(Value *args) {
    bool hasDouble = false;
    bool firstNum = true;

    double sub = 0;
    Value *cur = args;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);
        if(cur -> type == DOUBLE_TYPE) {
            hasDouble = true;
            sub -= cur -> d;
        }else if(cur -> type == INT_TYPE)  {
            sub -= cur -> i;
        } else {
            // Trying to add non-numeric type
            evaluationError("invalid type for =");
        }
        
        if(firstNum) {
            sub *= -1;
            firstNum = false;
        }

        cur = follow;
    }

    Value *result = talloc(sizeof(Value));
    if(hasDouble) {
        result -> type = DOUBLE_TYPE;
        result -> d = sub;
    } else {
        result -> type = INT_TYPE;
        // since sum is a double cast to int
        result -> i = (int) sub;
    }   

    return result;
}

Value *primitiveEqual(Value *args) {

    Value *first = car(args);
    Value *second = car(cdr(args));

    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;

    if(first -> type == INT_TYPE) {
        if(second -> type == INT_TYPE) {
            result -> i = first -> i == second -> i;

        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> i == second -> d;
        }
        else {
            evaluationError("= only takes numeric types");
        }
    } else if(first -> type == DOUBLE_TYPE)  {
        if(second -> type == INT_TYPE) {
            result -> i = first -> d == second -> i;
        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> d == second -> d;
        }
        else {
            evaluationError("= only takes numeric types");
        }
    } else {
        evaluationError("= only takes numeric types");
    }

    return result;
}

Value *primitiveLessThan(Value *args) {

    if(length(args) != 2) {
        evaluationError("< only takes two parameters.");
    }

    Value *first = car(args);
    Value *second = car(cdr(args));

    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;

    if(first -> type == INT_TYPE) {
        if(second -> type == INT_TYPE) {
            result -> i = first -> i < second -> i;

        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> i < second -> d;
        }
        else {
            evaluationError("< only takes numeric types");
        }
    } else if(first -> type == DOUBLE_TYPE)  {
        if(second -> type == INT_TYPE) {
            result -> i = first -> d < second -> i;
        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> d < second -> d;
        }
        else {
            evaluationError("< only takes numeric types");
        }
    } else {
        evaluationError("< only takes numeric types");
    }

    return result;
}

Value *primitiveGreaterThan(Value *args) {
    
    if(length(args) != 2) {
        evaluationError("> only takes two parameters.");
    }

    Value *first = car(args);
    Value *second = car(cdr(args));

    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;
    
    if(first -> type == INT_TYPE) {
        if(second -> type == INT_TYPE) {
            result -> i = first -> i > second -> i;

        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> i > second -> d;
        }
        else {
            evaluationError("> only takes numeric types");
        }
    } else if(first -> type == DOUBLE_TYPE)  {
        if(second -> type == INT_TYPE) {
            result -> i = first -> d > second -> i;
        } else if(second -> type == DOUBLE_TYPE) {
            result -> i = first -> d > second -> d;
        }
        else {
            evaluationError("> only takes numeric types");
        }
    } else {
        evaluationError("> only takes numeric types");
    }

    return result;
}

Value *primitiveMultiply(Value *args) {
    bool hasDouble = false;

    double product = 1;
    Value *cur = args;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);
        if(cur -> type == DOUBLE_TYPE) {
            hasDouble = true;
            product *= cur -> d;
        }else if(cur -> type == INT_TYPE)  {
            product *= cur -> i;
        } else {
            // Trying to add non-numeric type
            evaluationError("invalid type for *");
        }
        cur = follow;
    }

    Value *result = talloc(sizeof(Value));
    if(hasDouble) {
        result -> type = DOUBLE_TYPE;
        result -> d = product;
    } else {
        result -> type = INT_TYPE;
        // since sum is a double cast to int
        result -> i = (int) product;
    }   

    return result;

}

Value *primitiveDivision(Value *args) {

    Value *first = car(args);
    Value *second = car(cdr(args));

    double firstVal = 0;
    double secondVal = 0;

    if(first -> type == INT_TYPE) {
        firstVal = first -> i;
    } else {
        firstVal = first -> d;
    }
    if(second -> type == INT_TYPE) {
        secondVal = second -> i;
    } else {
        secondVal = second -> d;
    }

    double quotient = firstVal / secondVal;

    Value *result = talloc(sizeof(Value));

    if(quotient != (int) quotient) {
        result -> type = DOUBLE_TYPE;
        result -> d = quotient;
    } else {
        result -> type = INT_TYPE;
        result -> i = (int) quotient;
    }
    return result;
}

Value *primitiveModulo(Value *args) {

    // Assuming two arguments
    int first = car(args) -> i;
    int second = car(cdr(args)) -> i;
    
    int mod = first % second;
    Value *modResult = talloc(sizeof(Value));
    modResult -> type = INT_TYPE;
    modResult -> i = mod;

    return modResult;
}

void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    
    // Add primitive functions to top-level bindings list
    Value *value = talloc(sizeof(Value));
    value -> type = PRIMITIVE_TYPE;
    value -> pf = function;

    // Add symbol for primitive function in the frame
    Value *symbol = talloc(sizeof(Value));
    symbol -> type = SYMBOL_TYPE;
    symbol -> s = name;

    frame -> bindings = cons(value, frame -> bindings);
    frame -> bindings = cons(symbol, frame -> bindings);
}   

// ================================================
// ================= INTERPRET ===================
// ================================================
void printResult(Value *result) {

    switch (result -> type) {
        case BOOL_TYPE:
            if (result -> i == 1) {
                printf("#t ");
            }
            else {
                printf("#f ");
            }
            break;
        case INT_TYPE:
            printf("%i ", result -> i);
            break;
        case DOUBLE_TYPE:
            printf("%f ", result -> d);        
            break;
        case STR_TYPE:
            printf("%s ", result -> s);
            break;
        case SYMBOL_TYPE:
            printf("%s ", result -> s);
            break;
        case CONS_TYPE:
            printf("(");

            // Goes through cons sequence
            // This is so that parans are accurate
            while(!isNull(result)) {

                Value *follow = cdr(result);
                printResult(car(result));

                // Irregular cons type (i.e. pair)
                if (follow -> type != CONS_TYPE &&
                follow -> type != NULL_TYPE) {
                    printf(". ");
                    printResult(follow);
                    // Ends loop
                    break;
                }

                result = follow;

            }
            printf(") ");
            break;
        case NULL_TYPE:
            printf("() ");
            break;
        case CLOSURE_TYPE:
            printf("#<procedure> ");
            break;
        case VOID_TYPE:
            break;
        case UNSPECIFIED_TYPE:
            printf("UNSPECIFIED ");
            break;
        default:
            break;
    }
}

void interpret(Value *tree) {

    Value *cur = tree;

    Frame *global = talloc(sizeof(Frame));
    global -> bindings = makeNull();
    global -> parent = NULL;

    // Primitives
    bind("+", primitiveAdd, global);
    bind("null?", primitiveNull, global);
    bind("car", primitiveCar, global);
    bind("cdr", primitiveCdr, global);
    bind("cons", primitiveCons, global);

    bind("-", primitiveSubtraction, global);
    bind("=", primitiveEqual, global);
    bind("<", primitiveLessThan, global);
    bind(">", primitiveGreaterThan, global);
    bind("*", primitiveMultiply, global);
    bind("/", primitiveDivision, global);
    bind("modulo", primitiveModulo, global);

    Value *result;
    
    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        result = eval(car(cur), global);

        printResult(result);
        printf("\n");
        cur = follow;

    }

}

// ================================================
// ================= EVALUATION ===================
// ================================================
bool checkSymbolInFrame(Value *bindings, Value *var) {
    
    Value *cur = bindings;

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);
        if(!strcmp(cur -> s, var -> s)) {
            return true;
        }

        cur = cdr(follow);

    }
    return false;
    
}

Value *lookUpSymbol(Value *expr, Frame *frame) {
    
    Value *cur = frame -> bindings;
    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur); 

        if(!strcmp(cur -> s, expr -> s)) {
            return car(follow);
        }

        // Double skip
        cur = cdr(follow);

    }

    // If there is no parent for frame
    if (frame -> parent == NULL) {
        evaluationError("could not find symbol in frames");
    }

    return lookUpSymbol(expr, frame -> parent);
} 

Value *evalQuote(Value *args, Frame *frame){

    if(length(args) != 1) {
        evaluationError("incorrect number of arguments in quote");
    }
    
    return car(args);

}

Value *evalIf(Value *args, Frame *frame) {

    if(length(args) < 3) {
        evaluationError("incorrect number of arguments in if");
    }

    // (if expression trueExpr falseExpr)
    Value *expression = eval(car(args), frame);

    // expression must be BOOL_TYPE

    // if expression is true -> trueExpr 
    if (expression -> i == 1) {
        return car(cdr(args));
    // if expression false -> falseExpr
    } else {
        return car(cdr(cdr(args)));
    }

}

Value *evalLet(Value *args, Frame *frame) {

    // (let ((var1 expr1) (var2 expr2) ... (varn exprn)) body)

    if(length(args) < 2) {
        evaluationError("incorrect number of arguments in let");
    }


    // 1. Let e be a pointer to the current frame. 
    // Create a new frame f whose parent frame is e.
    Frame *e = frame;
    Frame *f = talloc(sizeof(Frame));
    f -> parent = e;
    f -> bindings = makeNull();

    // 2. For i = 1 through n:
    // - Let vali be the result of evaluating expri in frame e.
    // - Add a binding from vari to vali to f.
    
    Value *letExprs = car(args); // ((var1 expr1) (var2 expr2) ... (varn exprn))
    
    if (letExprs -> type != CONS_TYPE && letExprs -> type != NULL_TYPE) {
        evaluationError("incorrect format of args in let");
    }

    while (!isNull(letExprs)) {

        Value *thisExpr = car(letExprs); // (vari expri)
        
        if(length(args) < 2) {
            evaluationError("incorrect number of arguments in expr");
        }
        if (thisExpr -> type != NULL_TYPE ) {
            
            Value *vari = car(thisExpr); // vari

            if (vari -> type != SYMBOL_TYPE || 
            checkSymbolInFrame(f -> bindings, vari)) {
                evaluationError("trying to map value to non-symbol");
            }

            Value *expri = car(cdr(thisExpr)); // expri
            Value *vali = eval(expri, e);

            // Adding to frame
            f -> bindings = cons(vali, f -> bindings);
            f -> bindings = cons(vari, f -> bindings);

        }
        // Iterate 
        letExprs = cdr(letExprs);
    }
    
    // 3. Evaluate body using frame f and return the result.

    Value *body = cdr(args);
    Value *result;
    while(!isNull(body)) {

        Value *nextBody = cdr(body);
        body = car(body);
        result = eval(body, f);

        body = nextBody;

    }

    return result;
    
}

Value *evalLetStar(Value *args, Frame *frame){
    // (let ((var1 expr1) (var2 expr2) ... (varn exprn)) body)

    if(length(args) < 2) {
        evaluationError("incorrect number of arguments in let");
    }

    // 1. Let e be a pointer to the current frame. 
    // Create a new frame f whose parent frame is e.
    /*
    Frame *e = frame;
    Frame *f = talloc(sizeof(Frame));
    f -> parent = e;
    f -> bindings = makeNull();
    */

    // 2. For i = 1 through n:
    // - Let vali be the result of evaluating expri in frame e.
    // - Add a binding from vari to vali to f.
    
    Value *letExprs = car(args); // ((var1 expr1) (var2 expr2) ... (varn exprn))
    
    if (letExprs -> type != CONS_TYPE && letExprs -> type != NULL_TYPE) {
        evaluationError("incorrect format of args in let");
    }

    Frame *lastFrame = frame;
    Frame *f;
    
    while (!isNull(letExprs)) {

        // Create New Frame
        f = talloc(sizeof(Frame));
        f -> parent = lastFrame;
        f -> bindings = makeNull();

        Value *thisExpr = car(letExprs); // (vari expri)
        
        if(length(args) < 2) {
            evaluationError("incorrect number of arguments in expr");
        }

        if (thisExpr -> type != NULL_TYPE ) {
            
            Value *vari = car(thisExpr); // vari

            if (vari -> type != SYMBOL_TYPE || 
            checkSymbolInFrame(f -> bindings, vari)) {
                evaluationError("trying to map value to non-symbol");
            }

            Value *expri = car(cdr(thisExpr)); // expri
            Value *vali = eval(expri, lastFrame);

            // Adding to frame
            f -> bindings = cons(vali, f -> bindings);
            f -> bindings = cons(vari, f -> bindings);

        }
        // Iterate 
        letExprs = cdr(letExprs);

        // 
        lastFrame = f;
    }
    
    // 3. Evaluate body using frame f and return the result.

    Value *body = cdr(args);
    Value *result;
    
    while(!isNull(body)) {

        Value *nextBody = cdr(body);
        body = car(body);
        result = eval(body, f);

        body = nextBody;

    }

    return result;
    
}
Value *evalLetrec(Value *args, Frame *frame){
//(letrec ((x1 e1) ... (xn en)) body1 ... bodyn)

//Create a new frame env’ with parent env.
// Create each of the bindings, and set them to UNSPECIFIED_TYPE (this is in value.h).
// Evaluate each of e1, …, en in environment env’. If any of those evaluations use anything with UNSPECIFIED_TYPE, that should result in an error.
// After all of these evaluations are complete, replace bindings for each xi with the evaluated result of ei (from step 2) in environment env’.
// Evaluate body1, …, bodyn sequentially in env’, returning the result of evaluating bodyn.
    
    if(length(args) < 2) {
        evaluationError("incorrect number of arguments in let");
    }
    
    Frame *e = frame;
    Frame *f = talloc(sizeof(Frame));
    f -> parent = e;
    f -> bindings = makeNull();

    Value *letExprs = car(args); // ((var1 expr1) (var2 expr2) ... (varn exprn))
    
    if (letExprs -> type != CONS_TYPE && letExprs -> type != NULL_TYPE) {
        evaluationError("incorrect format of args in let");
    }


    // assign UNSPECIFIED_TYPE to all LH
    while(!isNull(letExprs)) {

        Value *LHS = car(car(letExprs));    
        if (LHS -> type != SYMBOL_TYPE) {
            
            evaluationError("trying to map value to non-symbol");
        }

        Value *unspec = talloc(sizeof(Value));
        unspec -> type = UNSPECIFIED_TYPE;

        f -> bindings = cons(unspec, f -> bindings);
        f -> bindings = cons(LHS, f -> bindings);

        // Iterate 
        letExprs = cdr(letExprs);

    }

    // evaluate RHS and put in a list
    Value *evaledRHS = talloc(sizeof(Value));
    evaledRHS = makeNull();
    letExprs = car(args);

    while(!isNull(letExprs)) {

        Value *RHS = car(cdr(car(letExprs)));
        RHS = eval(RHS, f);

        if(RHS -> type == UNSPECIFIED_TYPE) {
            evaluationError("returning #<unspecified>");
        }

        evaledRHS = cons(RHS, evaledRHS);

        // Iterate 
        letExprs = cdr(letExprs);
    }

    // Bind above together
    Value *curBindings = f -> bindings;
    Value *newBindings = talloc(sizeof(Value));
    newBindings = makeNull();

    while(!isNull(curBindings)) {
        Value *curLHS = car(curBindings);
        Value *curRHS = car(evaledRHS);

        newBindings = cons(curRHS, newBindings);
        newBindings = cons(curLHS, newBindings);

        // iterate through bindings and RHS

        curBindings = cdr(cdr(curBindings)); // Double iterate because of long list
        evaledRHS = cdr(evaledRHS);
    }
    //newBindings = reverse(newBindings);

    // Update frame bindings
    f -> bindings = newBindings;


    // 4. Evaluate body using frame f and return the result.

    Value *body = cdr(args);
    Value *result;
    
    while(!isNull(body)) {
        Value *nextBody = cdr(body);

        body = car(body);

        result = eval(body, f);
        body = nextBody;

    }
    return result;

}


//(define x 3)
Value *evalDef(Value *args, Frame *frame) {

    if(length(args) != 2) {
        evaluationError("incorrect number of arguments in define");
    }

    Value *key = car(args);
    Value *body = car(cdr(args));
    
    if (key -> type != SYMBOL_TYPE) {
        evaluationError("trying to bind non-symbol to value in define");
    }
    
    Value *val = eval(car(cdr(args)), frame);
        
    frame -> bindings = cons(val, frame -> bindings);
    frame -> bindings = cons(key, frame -> bindings);

    Value *temp = talloc(sizeof(Value));
    temp -> type = VOID_TYPE;

    return temp;
}

// (lambda params body)
Value *evalLambda(Value *args, Frame *frameEnv) {

    if(length(args) != 2) {       
        evaluationError("incorrect number of arguments in lambda");
    }

    // Make Closure
    Value *newClosure = talloc(sizeof(Value));
    newClosure -> type = CLOSURE_TYPE;

    // Check params
    Value *cur = car(args);
    Value *argCheck = makeNull();

    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);

        if(cur -> type != SYMBOL_TYPE) {
            evaluationError("params in lambda must be symbols");
        }

        // Checking for duplicate parameters
        Value *dupArgCheck = argCheck;
        while(!isNull(dupArgCheck)) {
            Value *dupFollow = cdr(dupArgCheck);
            dupArgCheck = car(dupArgCheck);
            if(!strcmp(dupArgCheck -> s, cur -> s)) {
                evaluationError("double literal in lambda");
            }
            dupArgCheck = dupFollow;
        }

        argCheck = cons(cur, argCheck);
        cur = follow;
    } 

    // Set closure params

    newClosure -> cl.frame = frameEnv;    
    newClosure -> cl.paramNames = car(args);
    newClosure -> cl.functionCode = car(cdr(args));

    return newClosure;

}


Value *apply(Value *function, Value *args) {

    // 1. Construct a new frame whose parent frame 
    // is the environment stored in the closure.
    
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame -> parent = function -> cl.frame;
    newFrame -> bindings = makeNull();

    // 2. Add bindings to the new frame mapping each formal
    // parameter (found in the closure) to the corresponding 
    // actual parameter (found in args).

    if(length(function -> cl.paramNames) != length(args)) {
        evaluationError("unvalid number of parameters in function.");
    }

    Value *curParam = function -> cl.paramNames;
    Value *curArg = args;

    while(!isNull(curParam)) {

        // Adding to params to the new frame frame
        newFrame -> bindings = cons(car(curArg), newFrame -> bindings);
        newFrame -> bindings = cons(car(curParam), newFrame -> bindings);

        curArg = cdr(curArg);
        curParam = cdr(curParam);
    }

    // 3. Evaluate the function body (found in the closure) 
    // with the new frame as its environment, 
    // and return the result of the call to eval.

    return eval(function -> cl.functionCode, newFrame);

}

Frame *findFrame(Value *expr, Frame *frame) {

    Value *cur = frame -> bindings;
    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur); 

        if(!strcmp(cur -> s, expr -> s)) {
            return frame;
        }

        // Double skip
        cur = cdr(follow);

    }

    // If there is no parent for frame
    if (frame -> parent == NULL) {
        evaluationError("could not find symbol in frames");
    }

    return findFrame(expr, frame -> parent);

}

Value *evalSet(Value *args, Frame *frame) {

    // find frame
    Value *symbol = car(args);
    Value *newValue = eval(car(cdr(args)), frame);

    Frame *defFrame = findFrame(symbol, frame);

    Value *cur = defFrame -> bindings;
    Value *newBindings = makeNull();

    while(!isNull(cur)) {

        Value *symVal = cdr(cur);
        cur = car(cur);

        if(!strcmp(cur -> s, symbol -> s)) {
            // Found symbol
            newBindings = cons(newValue, newBindings);
            newBindings = cons(cur, newBindings);

        }
        else {
            newBindings = cons(car(symVal), newBindings);
            newBindings = cons(cur, newBindings);
        }
        // Double skip
        cur = cdr(symVal);
    }

    defFrame -> bindings = newBindings;

    Value *temp = talloc(sizeof(Value));
    temp -> type = VOID_TYPE;

    return temp;

}

Value *evalBegin(Value *args, Frame *frame) {

    if(length(args) == 0) {
        Value *temp = talloc(sizeof(Value));
        temp -> type = VOID_TYPE;
        return temp;
    }

    Value *cur = args;
    Value *lastEval;

    while(!isNull(cur)) {

        lastEval = eval(car(cur), frame);
        cur = cdr(cur);

    }

    return lastEval;    
}

Value *evalAnd(Value *args, Frame *frame) {

    Value *cur = args;
    Value *evaledValue;
    
    while(!isNull(cur)) {
        evaledValue = eval(car(cur), frame);
        if(evaledValue -> type == BOOL_TYPE &&
            evaledValue -> i == 0) {
            return evaledValue;
        } 
        cur = cdr(cur);
        
    }

    return evaledValue;
    
}


Value *evalOr(Value *args, Frame *frame) {
    
    Value *cur = args;
    Value *evaledValue;
    
    while(!isNull(cur)) {
        evaledValue = eval(car(cur), frame);
        if(evaledValue -> type == BOOL_TYPE &&
            evaledValue -> i == 1) {
            return evaledValue;
        } 
        cur = cdr(cur);
    }
    return evaledValue;
}

Value *evalCond(Value *args, Frame *frame) {

    Value *cur = args;
    while(!isNull(cur)) {

        Value *follow = cdr(cur);
        cur = car(cur);
        
        if (car(cur) -> type == SYMBOL_TYPE) {
            // Assuming this is else
            return car(cdr(cur));

        } else {
            Value *boolTest = eval(car(cur), frame);
            if (boolTest -> i == 1) {
                // Return expr
                return car(cdr(cur));
            } 
        }
        // iteratre
        cur = follow;
    }

    Value *temp = talloc(sizeof(Value));
    temp -> type = VOID_TYPE;

    return temp;

}

Value *evalEach(Value *args, Frame *frame) {

    Value *evaledArgs = makeNull();
    Value *cur = args;

    while(!isNull(cur)) {
        Value *follow = cdr(cur);
        cur = car(cur);

        // evaluate and add 
        Value *newEvalArg = eval(cur, frame);
        evaledArgs = cons(newEvalArg, evaledArgs);

        cur = follow;

    }

    evaledArgs = reverse(evaledArgs);

    return evaledArgs;

}

// Evaluates an expression and its frame
Value *eval(Value *expr, Frame *frame) {

    switch (expr -> type)  {
            case VOID_TYPE:
            return expr;
            break;
        case INT_TYPE:
            return expr;
            break;
        case BOOL_TYPE:
            return expr;
            break;
        case DOUBLE_TYPE:
            return expr;
            break;
        case STR_TYPE:
            return expr;
            break;
        case SYMBOL_TYPE:
            return lookUpSymbol(expr, frame);
            break;
        case CLOSURE_TYPE:
            return expr;
            break;
        case PRIMITIVE_TYPE:
            return expr;
            break;
        case CONS_TYPE: {

            // First is the symbol (quote etc.)
            Value *first = car(expr);
            // The arguments
            Value *args = cdr(expr);

            Value *result;
            // SPECIAL CASES
            if(!strcmp(first -> s, "quote")) {  
                return evalQuote(args, frame);
            } else if (!strcmp(first -> s, "if")) {
                result = evalIf(args, frame);
            } else if (!strcmp(first -> s, "let*")) {
                return evalLetStar(args, frame);
            } else if (!strcmp(first -> s, "letrec")) {
                return evalLetrec(args, frame);
            } else if (!strcmp(first -> s, "let")) {
                return evalLet(args, frame);
            } else if (!strcmp(first -> s, "define")) {
                result = evalDef(args, frame); 
            } else if (!strcmp(first -> s, "lambda")) {
                result = evalLambda(args, frame); 
            } else if (!strcmp(first -> s, "set!")) {
                result = evalSet(args, frame); 
            } else if (!strcmp(first -> s, "begin")) {
                result = evalBegin(args, frame); 
            } else if (!strcmp(first -> s, "and")) {
                result = evalAnd(args, frame);
            } else if (!strcmp(first -> s, "or")) {
                result = evalOr(args, frame);
            } else if (!strcmp(first -> s, "cond")) {
                result = evalCond(args, frame);
            }
            // PRIMITIVES and USER DEF FUNCTIONS            
            else {
               
                // Evaluate first operator
                Value *evaledOperator = eval(first, frame);

                // Evaluate args
                Value *evaledArgs = evalEach(args, frame);

                // PRIMITIVES
                if (evaledOperator -> type == PRIMITIVE_TYPE) {
                    // Run primitve function
                    return evaledOperator -> pf(evaledArgs);
                } else {
                    // USER DEFINED FUNCTIONS
                    return apply(evaledOperator, evaledArgs);
                }
            }

            return eval(result, frame);
            break;
        }
        case NULL_TYPE:
            return expr;
            break;
        case UNSPECIFIED_TYPE:
            return expr;
            break;
        default:
            evaluationError("unknown value found.");
            return expr;
            break;
        }
}