#include "value.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Value valueNew(ValueType type, uint8_t *ptr, size_t len) {
  if (!type || !ptr || !len) {
    return (Value){.type = 0, .ptr = NULL, .len = 0};
  }
  uint8_t *newPtr = malloc(sizeof(uint8_t) * (len + 1));
  memcpy(newPtr, ptr, sizeof(uint8_t) * len);
  newPtr[len] = 0;
  return (Value){.type = type, .ptr = newPtr, .len = len};
}

Value *valueFree(Value *const value) {
  if (!value->ptr)
    return NULL;
  free(value->ptr);
  value->len = 0;
  return value;
}

bool valueIsStr(const Value value) { return value.type == ValueTypeString; }

bool valueIsNum(const Value value) { return !valueIsStr(value); }