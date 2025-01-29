
#include "lexer.h"
#include "token.h"
#include "vec.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Vec read(FILE *stream, const char *const delimiters) {
  Vec buf = VecOf(uint8_t);
  if (!buf.ptr) {
    perror("Allocation Error");
  }
  int8_t c = 0;
  while ((c = getc(stream)) != EOF && !strchr(delimiters, c))
    vecPush(&buf, &c);
  VecPushValue(&buf, uint8_t, '\0');
  vecShrinkToFit(&buf);
  return buf;
}

static uint8_t *readFile(FILE *file, size_t *read) {
  fseek(file, 0, SEEK_END);
  long bytesToRead = ftell(file);
  rewind(file);
  uint8_t *buf = malloc(sizeof(uint8_t) * (bytesToRead + 1));
  if (!buf) {
    perror("Reading Error");
    return NULL;
  }
  *read = fread(buf, sizeof(uint8_t), bytesToRead, file);
  if (*read != (size_t)bytesToRead) {
    free(buf);
    perror("Reading Error");
    return NULL;
  }
  buf[*read] = '\0';
  return buf;
}

static int eval(const uint8_t *const src, size_t len) {
  Lexer lex = lexerNew(src, len);
  Vec tokens = tokenize(&lex);
  VecForEach(tokens, Token, token, {
    tokenDebug(*token);
    tokenFree(token);
  });
  vecFree(&tokens);
  return EXIT_SUCCESS;
}

int repl(void) {
  Vec buf;
  for (;;) {
    printf("> ");
    buf = read(stdin, "\n");
    if (!buf.ptr) {
      perror("Error Allocating Memory");
      return -1;
    }
    if (feof(stdin)) {
      vecFree(&buf);
      return 64;
    }
    if (!strcmp(buf.ptr, "exit"))
      break;
    eval(buf.ptr, buf.len);
    vecFree(&buf);
  }
  vecFree(&buf);
  return EXIT_SUCCESS;
}

int script(const char *const restrict filename) {
  FILE *fptr = fopen(filename, "r");
  if (!fptr) {
    perror("Error Opening File");
    return EXIT_FAILURE;
  }
  size_t len = 0;
  uint8_t *buf = readFile(fptr, &len);
  int result = eval(buf, len);
  free(buf);
  return result;
}
