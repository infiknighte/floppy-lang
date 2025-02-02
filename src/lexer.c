#include "lexer.h"
#include "iter.h"
#include "token.h"
#include "vec.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

static uint8_t lexerNext(Lexer *const lex) {
  void *temp = iterNext(&lex->iter);
  return temp ? *(uint8_t *)temp : 0;
}

Lexer lexerNew(const uint8_t *const src, size_t len) {
  return (Lexer){.iter = iterFrom((void *)src, sizeof(uint8_t), len),
                 .hadError = false};
}

Token getToken(Lexer *const lex) {
  if (lex->hadError) {
    fputs("Call on `getToken`, after encountering an Error", stderr);
    exit(EXIT_FAILURE);
  }
  uint8_t c = 0;
  {
    void *temp = iterPeek(lex->iter);
    if (temp)
      c = *(uint8_t *)temp;
  }
  switch (c) {
  case 0:
    iterAdvance(&lex->iter);
    return tokenNewOp(TokenTypeEOF);
  case ';':
    c = lexerNext(lex);
    while (c != 0 && c != '\n')
      c = *(uint8_t *)iterAdvance(&lex->iter);
    return getToken(lex);
  case '"': {
    Vec value = VecOf(uint8_t);
    c = lexerNext(lex);
    size_t begin = lex->iter.cur;
    while (iterNotEnd(lex->iter) && c != '"') {
      if (c == '\\') {
        c = lexerNext(lex);
        switch (c) {
        case 'n':
          c = '\n';
          break;
        case 't':
          c = '\t';
          break;
        case '"':
          c = '"';
          break;
        case 'b':
          c = '\b';
          break;
        case 'r':
          c = '\r';
          break;
        case '\\':
          c = '\\';
          break;
        }
      }
      vecPush(&value, &c);
      c = lexerNext(lex);
    }
    iterAdvance(&lex->iter); // Skipping `"` delimeter
    VecPushValue(&value, uint8_t, '\0');
    return tokenNew(TokenTypeString, value.ptr, lex->iter.cur - begin - 1);
  }
  case '\n':
    iterAdvance(&lex->iter);
    return tokenNewOp(TokenTypeEOL);
  case ' ':
  case '\t':
  case '\v':
  case '\r':
  case '\f':
    c = lexerNext(lex);
    while (iterNotEnd(lex->iter) && isBlank(*lex)) {
      c = lexerNext(lex);
    }
    return getToken(lex);
  default:
    if (isdigit(c)) {
      bool isInt = true;
      bool hasDot = false;
      bool isUnderscore = false;
      Vec value = VecOf(uint8_t);
      vecPush(&value, &c);
      size_t begin = lex->iter.cur;
      c = lexerNext(lex);
      while (isdigit(c) || ((hasDot = c == '.') && isInt) ||
             (isUnderscore = c == '_')) {
        if (hasDot)
          isInt = false;
        if (isUnderscore)
          continue;
        vecPush(&value, &c);
        c = lexerNext(lex);
      }
      VecPushValue(&value, uint8_t, '\0');
      return tokenNew(isInt ? TokenTypeInt : TokenTypeFloat, value.ptr,
                      lex->iter.cur - begin);
    }
    if (isalpha(c)) {
      Vec identStr = VecOf(uint8_t);
      vecPush(&identStr, &c);
      size_t begin = lex->iter.cur;
      c = lexerNext(lex);
      while (iterNotEnd(lex->iter) && (isalpha(c) || isdigit(c) || c == '_')) {
        vecPush(&identStr, &c);
        c = lexerNext(lex);
      }
      VecPushValue(&identStr, uint8_t, '\0');
      TokenType opCode = 0;
      const char *const restrict KEYWORDS[KEYWORDS_NUMBER] = KEYWORDS_RVALUE;
      for (opCode = 0; opCode < KEYWORDS_NUMBER; ++opCode)
        if (!strcmp(identStr.ptr, KEYWORDS[opCode]))
          break;
      if (opCode != KEYWORDS_NUMBER) {
        vecFree(&identStr);
        return tokenNewOp(opCode);
      }
      return tokenNew(TokenTypeIdent, identStr.ptr, lex->iter.cur - begin);
    }
    iterAdvance(&lex->iter);
    lex->hadError = true;
    uint8_t *ident = calloc(2, sizeof(uint8_t));
    ident[0] = c;
    return tokenNew(TokenTypeUnknown, ident, 1);
  }
}

Vec tokenize(Lexer *const lex) {
  Vec tokens = VecOfWith(Token, lex->iter.len / 2);
  while (iterNotEnd(lex->iter))
    VecPushValue(&tokens, Token, getToken(lex));
  VecPushValue(&tokens, Token, tokenNewOp(TokenTypeEOF));
  return tokens;
}