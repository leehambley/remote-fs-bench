// For access to strdup() (see man (3) strdup)
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <libgen.h>
#include <linux/limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* mkdir(2) */
#include <unistd.h>

char const USAGE[] = "Usage: server [OPTIONS]\n"
                     "Examples:\n"
                     "       TODO WRITE ME\n";

// In my project the average file size is about 4560 bytes.
// $ ls -l | gawk '{sum += $5; n++;} END {print sum/n;}'
// 4559.29
#define FILE_BYTE_SIZE 4560

#define SHARED_DIR_PATH_PREFIX "./shared-directory"

// should be customizable on the CLI
#define RANDOM_SEED 88373

// intended to be representative of a typical
// project for Ruby or whatever else
#define FILES_TO_CREATE 10000

// Must be less than or FILENAME_BUF_LEN. Don't forget to
// leave space for the zero terminator.
#define FILENAME_LEN 64

// From
// https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
int mkdir_p(const char *path) {
  /* Adapted from http://stackoverflow.com/a/2336245/119527 */
  const size_t len = strlen(path);
  char _path[PATH_MAX];
  char *p;

  errno = 0;

  /* Copy string so its mutable */
  if (len > sizeof(_path) - 1) {
    errno = ENAMETOOLONG;
    return -1;
  }
  strcpy(_path, path);

  /* Iterate the string */
  for (p = _path + 1; *p; p++) {
    if (*p == '/') {
      /* Temporarily truncate */
      *p = '\0';

      if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
          return -1;
      }

      *p = '/';
    }
  }

  if (mkdir(_path, S_IRWXU) != 0) {
    if (errno != EEXIST)
      return -1;
  }

  return 0;
}

int main(int argc, char const *argv[]) {

  // TODO: make seed configurable
  srand(RANDOM_SEED);

  // Collect a list of our generated files
  // to avoid unnecessary filesystem traversals.
  char *all_filenames[FILES_TO_CREATE] = {};

  // outer loop generates our thousands of files.
  for (unsigned int i = 0; i <= FILES_TO_CREATE; i++) {
    // at random directory depth between
    // three and fifteen
    unsigned int lower = 3;
    unsigned int upper = 15;
    unsigned int depth = (rand() % (upper - lower + 1)) + lower;

    // generate a random file-name, we'll inject (overwire)
    // directory separators in at specific positions
    char str[FILENAME_LEN + 1] = {0};
    for (unsigned int j = 0; j < FILENAME_LEN; j++) {
      sprintf(str + j, "%x", rand() % 16);
    }

    // depending on the depth, inject some directory
    // separators, we'll use dirname(2) and mkdirp(2) to
    // create the directory to contain the thing soon.
    // Note: depth refers to number of _directories_
    // the file will be at depth+1
    for (unsigned int k = 0; k <= depth; k++) {
      str[k * 3] = '/';
    }

    // Push the newly created filename onto the list of all
    // files. this is to stop us needing to do filesystem
    // traversal, and potentially generating filesystem events
    // in the container - we can just randomly select from
    // our list. Beware strdup copies including until the first \0
    // this _should_ be our whole string, but be-warned of
    // off-by-ones.
    all_filenames[i] = strdup(str);
    if (all_filenames[i] == NULL) {
      fprintf(stderr, "Error allocating memory in strdup\n");
      exit(1);
    }

    // separate basename and dirname, we'll mkdir_p the
    // dirname, we'll then open and create the file and
    // write FILE_BYTE_SIZE bytes from /dev/zero into
    // our file.
    // TODO: string manipulation ftw? Yikes this is ugly.
    char *str_basename = malloc(
        sizeof(char) * (strlen(SHARED_DIR_PATH_PREFIX) + FILENAME_LEN + 1));
    strcat(str_basename, SHARED_DIR_PATH_PREFIX);
    strcat(str_basename, basename(str));
    if (str_basename == NULL) {
      fprintf(stderr, "getting basename from %s\n", str);
      exit(1);
    }
    // TODO: string manipulation ftw? Yikes this is ugly.
    char *str_dirname = malloc(
        sizeof(char) * (strlen(SHARED_DIR_PATH_PREFIX) + FILENAME_LEN + 1));
    strcat(str_dirname, SHARED_DIR_PATH_PREFIX);
    strcat(str_dirname, dirname(str));
    if (str_dirname == NULL) {
      fprintf(stderr, "error getting dirname for %s\n", str);
      exit(1);
    }

    printf("%05d - %04d - %s - %s- %s\n", i, depth, str_basename, str_dirname,
           str);
    if (mkdir_p(str_dirname) != 1) {
      fprintf(stderr, "error creating directory %s\n", str_dirname);
      if (errno != EEXIST) {
        exit(2);
      }
    }
  }

  // print all the filenames we know about
  // for (unsigned int i = 0; i <= FILES_TO_CREATE; i++) {
  //   printf("%s\n", all_filenames[i]);
  // }

  return 0;
}
