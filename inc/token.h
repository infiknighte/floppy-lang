#pragma once

#include <stdint.h>
#include <stdlib.h>

#define KEYWORDS_NUMBER 19
#define KEYWORDS_RVALUE                                                        \
  {                                                                            \
    "put", "pop", "dup", "add", "sub", "and", "or", "xor", "not", "jmp",       \
        "jmpz", "jmpnz", "halt", "read", "print", "do", "def", "ret", "end",   \
  }

typedef enum : uint8_t {
  // Stack-based
  TokenTypePut = 0,
  TokenTypePop,
  TokenTypeDup,

  // Arithmedic
  TokenTypeAdd,
  TokenTypeSub,

  // Bitwise & Logical
  TokenTypeAnd,
  TokenTypeOr,
  TokenTypeXor,
  TokenTypeNot,

  // Control Flow
  TokenTypeJmp,
  TokenTypeJmpZ,
  TokenTypeJmpNZ,
  TokenTypeHalt,

  // IO
  TokenTypeRead,
  TokenTypePrint,

  // Labels
  TokenTypeDo,
  TokenTypeDef,
  TokenTypeRet,
  TokenTypeEnd,

  TokenTypeInt,
  TokenTypeFloat,
  TokenTypeString,
  TokenTypeIdent,

  TokenTypeEOL,
  TokenTypeEOF,
  TokenTypeUnknown,
} TokenType;

typedef struct {
  TokenType type;
  uint8_t *value;
  size_t len;
} Token;

Token tokenNew(TokenType type, uint8_t *value, size_t len);
Token tokenNewOp(TokenType type);
void tokenFree(Token *const token);
bool tokenIsLiteral(const Token token);
bool tokenIsSpecail(const Token token);
bool tokenIsOp(const Token token);
void tokenDebug(const Token token);
