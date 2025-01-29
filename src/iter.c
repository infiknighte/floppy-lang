#include "iter.h"
#include <stdint.h>
#include <stdio.h>

static inline bool inBounds(const Iter iter) {
  return iter.cur >= 0 && iter.cur < iter.len;
}

Iter iterNew(void *const src, size_t size, size_t len) {
  return (Iter){.src = src, .size = size, .len = len, .cur = 0};
}

void iterFree(Iter *const restrict iter) {
  free(iter->src);
  iter->size = 0;
  iter->len = 0;
  iter->cur = 0;
}

bool iterNotEnd(const Iter iter) { return iter.cur < iter.len; }

void *iterPeek(const Iter iter) {
  return inBounds(iter) ? iter.src + (iter.size * iter.cur) : NULL;
}

void *iterAdvance(Iter *const restrict iter) {
  void *pItem = iterPeek(*iter);
  if (iterNotEnd(*iter))
    iter->cur++;
  return pItem;
}
