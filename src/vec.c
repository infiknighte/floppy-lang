#include "vec.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static inline size_t vecNewCapacity(size_t capacity) {
  return (size_t)(capacity ? ((capacity * 3) / 2) : VEC_INIT_CAPACITY);
}

static inline bool inBounds(const Vec vec, size_t index) {
  return index >= 0 && index < vec.len;
}

static inline void *vecAt(const Vec vec, size_t index) {
  return vec.ptr + (vec.size * index);
}

Vec vecNew(size_t size) { return vecNewWith(size, VEC_INIT_CAPACITY); }

Vec vecNewWith(const size_t size, size_t capacity) {
  capacity = capacity < 8 ? VEC_INIT_CAPACITY : capacity;
  return (Vec){
      .ptr = malloc(size * capacity), .size = size, .len = 0, .cap = capacity};
}

Vec *vecRealloc(Vec *vec, const size_t newCap) {
  if (newCap < 0)
    return NULL;
  void *newPtr = realloc(vec->ptr, newCap * vec->size);
  if (!newPtr) {
    perror("Vec Reallocation Error");
    return NULL;
  }
  vec->ptr = newPtr;
  vec->cap = newCap;
  return vec;
}

void *vecGetBorrow(const Vec vec, size_t index) {
  return inBounds(vec, index) ? vecAt(vec, index) : NULL;
}

void *vecGetOwn(const Vec vec, size_t index, void *restrict dest) {
  return inBounds(vec, index) ? memcpy(dest, vecGetBorrow(vec, index), vec.size)
                              : NULL;
}

Vec *vecPush(Vec *const restrict vec, const void *restrict item) {
  if (!item) {
    perror("Cannot push NULL item");
    return NULL;
  }
  if (vec->len >= vec->cap) {
    size_t newCap = vecNewCapacity(vec->cap);
    void *newPtr = realloc(vec->ptr, vec->size * newCap);
    if (!newPtr)
      return NULL;
    vec->ptr = newPtr;
    vec->cap = newCap;
  }
  void *newElem = vecAt(*vec, vec->len);
  memcpy(newElem, item, vec->size);
  ++vec->len;
  return vec;
}

Vec *vecPop(Vec *const restrict vec) {
  if (vec->len <= 0)
    return NULL;
  --vec->len;
  return vec;
}

Vec *vecClear(Vec *const restrict vec) {
  vec->len = 0;
  return vec;
}

inline Vec *vecShrinkToFit(Vec *vec) { return vecRealloc(vec, vec->len); }

void vecFree(Vec *const restrict vec) {
  free(vec->ptr);
  vec->ptr = NULL;
  vec->size = vec->len = vec->cap = 0;
}
