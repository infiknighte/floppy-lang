#include "token.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token tokenNew(TokenType type, uint8_t *value, size_t len) {
  switch (type) {
  case TokenTypeInt:
  case TokenTypeFloat:
  case TokenTypeString:
  case TokenTypeIdent:
  case TokenTypeUnknown:
    return (Token){.type = type, .value = value, .len = len};
  default:
    return (Token){.type = type, .value = NULL, .len = 0};
  }
}

Token tokenNewOp(TokenType op) {
  switch (op) {
  case TokenTypeInt:
  case TokenTypeFloat:
  case TokenTypeString:
  case TokenTypeIdent:
  case TokenTypeUnknown:
    return tokenNew(TokenTypeUnknown, NULL, 0);
  default:
    return tokenNew(op, NULL, 0);
  }
}

void tokenFree(Token *const token) {
  if (token->value) {
    free(token->value);
    token->value = NULL;
  }
  token->type = TokenTypeUnknown;
}

bool tokenIsLiteral(const Token token) {
  switch (token.type) {
  case TokenTypeInt:
  case TokenTypeFloat:
  case TokenTypeString:
  case TokenTypeIdent:
    return true;
  default:
    return false;
  }
}

bool tokenIsSpecail(const Token token) {
  switch (token.type) {
  case TokenTypeEOL:
  case TokenTypeEOF:
  case TokenTypeUnknown:
    return true;
  default:
    return false;
  }
}

bool tokenIsOp(const Token token) {
  return !(tokenIsLiteral(token) && tokenIsSpecail(token));
}


void tokenDebug(const Token token) {
  printf("Token (Op %hhu) - ", token.type);
  switch (token.type) {
  case TokenTypePut:
    puts("PUT");
    break;
  case TokenTypePop:
    puts("POP");
    break;
  case TokenTypeDup:
    puts("DUP");
    break;
  case TokenTypeAdd:
    puts("ADD");
    break;
  case TokenTypeSub:
    puts("SUB");
    break;
  case TokenTypeAnd:
    puts("AND");
    break;
  case TokenTypeOr:
    puts("OR");
    break;
  case TokenTypeXor:
    puts("XOR");
    break;
  case TokenTypeNot:
    puts("NOT");
    break;
  case TokenTypeJmp:
    puts("JUMP");
    break;
  case TokenTypeJmpZ:
    puts("JUMP IF 0");
    break;
  case TokenTypeJmpNZ:
    puts("JUMP IF NOT 0");
    break;
  case TokenTypeHalt:
    puts("HALT");
    break;
  case TokenTypeRead:
    puts("READ");
    break;
  case TokenTypePrint:
    puts("PRINT");
    break;
  case TokenTypeDo:
    puts("DO");
    break;
  case TokenTypeDef:
    puts("DEFINE");
    break;
  case TokenTypeRet:
    puts("RETURN");
    break;
  case TokenTypeEnd:
    puts("END");
    break;
  case TokenTypeInt:
    printf("INT %s\n", token.value);
    break;
  case TokenTypeFloat:
    printf("FLOAT %s\n", token.value);
    break;
  case TokenTypeString:
    printf("STRING \"%s\"\n", token.value);
    break;
  case TokenTypeIdent:
    printf("Ident `%s`\n", token.value);
    break;
  case TokenTypeEOF:
    puts("EOF");
    break;
  case TokenTypeEOL:
    puts("EOL");
    break;
  case TokenTypeUnknown:
    printf("!! UNKNOWN !!\nUnknown identifier `%s`\n",
           token.value ? (char *)token.value : "");
    break;
  }
}
