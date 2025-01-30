#pragma once

#include "iter.h"
#include "token.h"
#include "vec.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  Iter iter;
  bool hadError;
} Lexer;

Lexer lexerNew(const uint8_t *const src, size_t len);
Token getToken(Lexer *const lex);
Vec tokenize(Lexer *const lex);
