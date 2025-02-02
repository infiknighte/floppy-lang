#include "stack.h"
#include "iter.h"
#include "token.h"
#include "value.h"
#include "vec.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline Token stackNext(Stack *const restrict stack) {
  Token *temp = iterNext(&stack->tokens);
  return temp ? *temp : tokenNewOp(TokenTypeEOF);
}

static inline Token stackPeek(const Stack stack) {
  return *(Token *)iterPeek(stack.tokens);
}

static inline Value stackPop(Stack *stack) {
  if (stack->values.len <= 0) {
    stack->hadError = true;
    return valueNew(0, NULL, 0);
  }
  Value *value = vecGetBorrow(stack->values, stack->values.len - 1);
  vecPop(&stack->values);
  return *value;
}

static inline void stackPut(Stack *stack, Value value) {
  VecPushValue(&stack->values, Value, value);
}

Stack stackNew(Iter tokens) {
  return (Stack){.values = VecOf(Value), .tokens = tokens, .hadError = false};
}

Stack *stackFree(Stack *stack) {
  VecForEach(stack->values, Value, value, valueFree(value););
  vecFree(&stack->values);
  stack->hadError = false;
  return stack;
}

void stackEval(Stack *const stack) {
  Token token = stackPeek(*stack);
  switch (token.type) {
  case TokenTypePut:
    token = stackNext(stack);
    stackPut(stack, valueNew((ValueType)token.type, token.value, token.len));
    break;
  case TokenTypePop:
    stackNext(stack);
    Value value = stackPop(stack);
    valueFree(&value);
    break;
  case TokenTypeDup:
    stackNext(stack);
    Value top = *(Value *)vecGetBorrow(stack->values, stack->values.len - 1);
    stackPut(stack, valueNew(top.type, top.ptr, top.len));
    break;
  case TokenTypeAdd: {
    stackNext(stack);
    Value rhs = stackPop(stack);
    Value lhs = stackPop(stack);
    if (valueIsStr(lhs) || valueIsStr(rhs)) {
      lhs.len += rhs.len + 1;
      lhs.ptr = realloc(lhs.ptr, lhs.len);
      strcat((char *)lhs.ptr, (char *)rhs.ptr);
      lhs.type = ValueTypeString;
      valueFree(&rhs);
      stackPut(stack, lhs);
      break;
    }
    switch (lhs.type) {
    case ValueTypeInt: {
      int32_t result = atoi((char *)lhs.ptr) + atoi((char *)rhs.ptr);
      uint8_t ptr[11];
      uint8_t len = sprintf((char *)ptr, "%d", result);
      valueFree(&lhs);
      valueFree(&rhs);
      Value temp = valueNew(ValueTypeInt, ptr, len);
      stackPut(stack, temp);
      break;
    }
    case ValueTypeFloat: {
      double result =
          strtod((char *)lhs.ptr, NULL) + strtod((char *)rhs.ptr, NULL);
      uint8_t ptr[51];
      uint8_t len = sprintf((char *)ptr, "%lf", result);
      valueFree(&rhs);
      valueFree(&lhs);
      stackPut(stack, valueNew(ValueTypeFloat, ptr, len));
      break;
    }
    default:
      break;
    }
  }
  case TokenTypeSub: {
    stackNext(stack);
    Value rhs = stackPop(stack);
    Value lhs = stackPop(stack);
    if (valueIsStr(lhs) || valueIsStr(rhs)) {
      // TODO!
      break;
    }
    switch (lhs.type) {
    case ValueTypeInt: {
      int32_t result = atoi((char *)lhs.ptr) - atoi((char *)rhs.ptr);
      uint8_t ptr[11];
      uint8_t len = sprintf((char *)ptr, "%d", result);
      ptr[len] = 0;
      valueFree(&lhs);
      valueFree(&rhs);
      stackPut(stack, valueNew(ValueTypeInt, ptr, len));
      break;
    }
    case ValueTypeFloat: {
      double result =
          strtod((char *)lhs.ptr, NULL) - strtod((char *)rhs.ptr, NULL);
      uint8_t ptr[51];
      uint8_t len = sprintf((char *)ptr, "%lf", result);
      ptr[len] = 0;
      valueFree(&rhs);
      valueFree(&lhs);
      stackPut(stack, valueNew(ValueTypeFloat, ptr, len));
      break;
    }
    default:
      break;
    }
    break;
  }
  case TokenTypeAnd:
  case TokenTypeOr:
  case TokenTypeXor:
  case TokenTypeNot:
  case TokenTypeJmp:
  case TokenTypeJmpZ:
  case TokenTypeJmpNZ:
  case TokenTypeHalt:
  case TokenTypeRead:
    stackNext(stack);
    break;
  case TokenTypePrint: {
    token = stackNext(stack);
    switch (token.type) {
    case TokenTypeInt:
    case TokenTypeFloat:
    case TokenTypeString:
      printf("%s\n", token.value);
      stackNext(stack);
      break;
    case TokenTypeEOL:
    case TokenTypeEOF:
      stackNext(stack);
      Value value = stackPop(stack);
      printf("%d - %zu\n", value.type, value.len);
      printf("%s\n\n", value.ptr);
      valueFree(&value);
      break;
    default:
      stack->hadError = true;
      return;
    }
    break;
  }
  case TokenTypeDo:
  case TokenTypeDef:
  case TokenTypeRet:
  case TokenTypeEnd:
  case TokenTypeInt:
  case TokenTypeFloat:
  case TokenTypeString:
  case TokenTypeIdent:
  case TokenTypeEOL:
  case TokenTypeUnknown:
    stackNext(stack);
    break;
  case TokenTypeEOF:
    stackNext(stack);
    break;
  }
}

void stackRun(Stack *stack) {
  while (iterNotEnd(stack->tokens))
    stackEval(stack);
}