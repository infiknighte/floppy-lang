#pragma once

#include <stdlib.h>

#define VEC_INIT_CAPACITY ((size_t)8)

#define VecOf(type) vecNew(sizeof(type))

#define VecOfWith(type, capacity) vecNewWith(sizeof(type), capacity)

#define VecPushValue(vec, type, value)                                         \
  ({                                                                           \
    type item = value;                                                         \
    vecPush(vec, &item);                                                       \
  })

#define VecForEach(vec, type, item, codeBlock)                                 \
  ({                                                                           \
    if ((vec).len > 0) {                                                       \
      for (size_t item##Index = 0; item##Index < (vec).len; ++item##Index) {   \
        type *item = vecGetBorrow(vec, item##Index);                           \
        codeBlock                                                              \
      }                                                                        \
    }                                                                          \
  })

typedef struct {
  void *ptr; // Owned
  size_t size;
  size_t len;
  size_t cap;
} Vec;

Vec vecNew(size_t size);
Vec vecNewWith(size_t size, size_t capacity);
Vec *vecRealloc(Vec *vec, const size_t newCap);
void *vecGetBorrow(const Vec vec, size_t index);
void *vecGetOwn(const Vec vec, size_t index, void *restrict dest);
Vec *vecPush(Vec *const restrict vec, const void *restrict item);
Vec *vecPop(Vec *const restrict vec);
Vec *vecClear(Vec *const restrict vec);
Vec *vecShrinkToFit(Vec *vec);
void vecFree(Vec *const restrict vec);
