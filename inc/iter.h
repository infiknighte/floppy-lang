#pragma once

#include <stdlib.h>

typedef struct {
  void *src;
  size_t size;
  size_t len;
  size_t cur;
} Iter;

Iter iterFrom(void *src, size_t size, size_t len);
bool iterNotEnd(const Iter iter);
void *iterPeek(const Iter iter);
void *iterAdvance(Iter *const restrict iter);
void *iterNext(Iter *const restrict iter);
void iterEnd(Iter *const iter);
