#include "io.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  switch (argc) {
  case 1:
    return repl();
  case 2:
    return script(argv[1]);
  default:
    fprintf(stderr,
            "Error: Given too much argument, got %d\nUsage: floppy "
            "<script>\n",
            argc - 1);
    return 1;
  }
}
