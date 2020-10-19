#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char const USAGE[] = "Usage: server [OPTIONS]\n"
                     "Examples:\n"
                     "       TODO WRITE ME\n";

// Buffer size to store file-names in when generating
// the directory structure seed. We'll generate a file
// name shorter than this, and then inject / at a few
// off-sets to generate a deeply nested file tree.
#define FILENAME_BUF_LEN 128

// Must be less than or FILENAME_BUF_LEN. Don't forget to
// leave space for the zero terminator.
#define FILENAME_LEN 64

int main(int argc, char const *argv[]) {

  // TODO: make seed configurable
  srand(88373);

  // outer loop generates 10,000 files.
  for (int i = 0; i <= 10000; i++) {
    // at random directory depth between
    // three and fifteen
    unsigned int lower = 3;
    unsigned int upper = 15;
    unsigned int depth = (rand() % (upper - lower + 1)) + lower;

    // generate a random file-name, we'll inject (overwire)
    // directory separators in at specific positions
    char str[FILENAME_LEN + 1] = {};
    for (int j = 0; j < FILENAME_LEN; j++) {
      sprintf(str + j, "%x", rand() % 16);
    }

    printf("%05d - %04d - %s\n", i, depth, str);
  }

  return 0;
}
