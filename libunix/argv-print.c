#include "libunix.h"

void argv_print(const char *msg, char *argv[]) {
    output("%s: argv=%s -> ", msg, argv[0]);
    for(int i =1; argv[i]; i++)
        output(" argv[%d]=<%s> ", i, argv[i]);
    output("\n");
}
