#pragma once

#include "iter.h"
#include "vec.h"
#include <stdint.h>

typedef struct {
  Vec values;
  Iter tokens;
  bool hadError;
} Stack;

Stack stackNew(Iter tokens);
Stack *stackFree(Stack *stack);
void stackRun(Stack *stack);
