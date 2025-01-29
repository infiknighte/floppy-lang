#include "lexer.h"
#include "token.h"
#include "vec.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool isBlank(const Lexer lex) {
  switch (*(uint8_t *)iterPeek(lex.iter)) {
  case ' ':
  case '\t':
  case '\v':
  case '\r':
  case '\f':
    return true;
  default:
    return false;
  }
}

Lexer lexerNew(const uint8_t *const src, size_t len) {
  return (Lexer){.iter = iterNew((void *)src, sizeof(uint8_t), len),
                 .hadError = false};
}

void lexerFree(Lexer *const restrict lex) {
  iterFree(&lex->iter);
  lex->hadError = false;
}

Token getToken(Lexer *const lex) {
  if (lex->hadError) {
    fputs("Call on `getToken`, after encountering an Error", stderr);
    exit(EXIT_FAILURE);
  }
  uint8_t c = 0;
  {
    void *temp = iterAdvance(&lex->iter);
    if (temp)
      c = *(uint8_t *)temp;
  }
  switch (c) {
  case '\0':
    return tokenNew(TokenTypeEOF, NULL);
  case ';':
    c = *(uint8_t *)iterAdvance(&lex->iter);
    while (c != 0 && c != '\n')
      c = *(uint8_t *)iterAdvance(&lex->iter);
    return getToken(lex);
  case '"': {
    Vec value = VecOf(uint8_t);
    c = *(uint8_t *)iterAdvance(&lex->iter);
    while (c != 0 && c != '"') {
      vecPush(&value, &c);
      c = *(uint8_t *)iterAdvance(&lex->iter);
    }
    iterAdvance(&lex->iter); // Skipping `"` delimeter
    VecPushValue(&value, uint8_t, '\0');
    return tokenNew(TokenTypeString, value.ptr);
  }
  case '\n':
    return tokenNew(TokenTypeEOL, NULL);
  case ' ':
  case '\t':
  case '\v':
  case '\r':
  case '\f':
    c = *(uint8_t *)iterAdvance(&lex->iter);
    while (c != 0 && isBlank(*lex))
      c = *(uint8_t *)iterAdvance(&lex->iter);
    return getToken(lex);
  default:
    if (isdigit(c)) {
      bool isInt = true;
      bool hasDot = false;
      bool isUnderscore = false;
      Vec value = VecOf(uint8_t);
      vecPush(&value, &c);
      for (c = *(uint8_t *)iterAdvance(&lex->iter);
           isdigit(c) || ((hasDot = c == '.') && isInt) ||
           (isUnderscore = c == '_');
           c = *(uint8_t *)iterAdvance(&lex->iter)) {
        if (hasDot) {
          isInt = false;
        }
        if (isUnderscore) {
          continue;
        }
        vecPush(&value, &c);
      }
      VecPushValue(&value, uint8_t, '\0');
      return tokenNew(isInt ? TokenTypeInt : TokenTypeFloat, value.ptr);
    }
    if (isalpha(c)) {
      Vec identStr = VecOf(uint8_t);
      vecPush(&identStr, &c);
      for (c = *(uint8_t *)iterAdvance(&lex->iter);
           isalpha(c) || isdigit(c) || c == '_';
           c = *(uint8_t *)iterAdvance(&lex->iter))
        vecPush(&identStr, &c);
      VecPushValue(&identStr, uint8_t, '\0');
      TokenType opCode = 0;
      const char *const restrict KEYWORDS[KEYWORDS_NUMBER] = KEYWORDS_RVALUE;
      for (opCode = 0; opCode < KEYWORDS_NUMBER; ++opCode) {
        if (!strcmp(identStr.ptr, KEYWORDS[opCode]))
          break;
      }
      if (opCode != KEYWORDS_NUMBER) {
        vecFree(&identStr);
        return tokenNew(opCode, NULL);
      }
      return tokenNew(TokenTypeIdent, identStr.ptr);
    }
    lex->hadError = true;
    uint8_t *ident = calloc(2, sizeof(uint8_t));
    ident[0] = c;
    return tokenNew(TokenTypeUnknown, ident);
  }
}

Vec tokenize(Lexer *const lex) {
  Vec tokens = VecOfWith(Token, lex->iter.len / 2);
  while (iterNotEnd(lex->iter))
    VecPushValue(&tokens, Token, getToken(lex));
  VecPushValue(&tokens, Token, tokenNew(TokenTypeEOF, NULL));
  return tokens;
}