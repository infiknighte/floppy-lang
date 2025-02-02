#pragma once

#include "token.h"
#include <stdint.h>
#include <stdlib.h>

typedef enum : uint8_t {
  ValueTypeInt = TokenTypeInt,
  ValueTypeFloat = TokenTypeFloat,
  ValueTypeString = TokenTypeString,
} ValueType;

typedef struct {
  ValueType type;
  uint8_t *ptr;
  size_t len;
} Value;

Value valueNew(ValueType type, uint8_t *ptr, size_t len);
Value *valueFree(Value *const value);
bool valueIsStr(const Value value);
bool valueIsNum(const Value value);
