#include <unistd.h>

char const USAGE[] = "Usage: server [FLAGS]\n"
                     "Examples:\n"
                     "   fanotify CLOSE_WRITE\n"
                     "       Watch the current directory\n"
                     "   fanotify CLOSE_WRITE MOUNT\n"
                     "       Watch the current filesytem\n"
                     "   fanotify CLOSE_WRITE \"\" /home\n"
                     "       Watch the /home directory\n";

int main(int argc, char const *argv[]) { return 0; }
