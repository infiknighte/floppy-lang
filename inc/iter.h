#pragma once

#include <stdlib.h>

typedef struct {
  void *src;
  size_t size;
  size_t len;
  size_t cur;
} Iter;

Iter iterNew(void *src, size_t size, size_t len);
void iterFree(Iter *const restrict iter);
bool iterNotEnd(const Iter iter);
void *iterPeek(const Iter iter);
void *iterAdvance(Iter *const restrict iter);
