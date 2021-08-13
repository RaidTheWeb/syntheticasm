#include <stdio.h>
#include <sys/stat.h>

#include "assembler.h"
#include "common.h"

static bool file_exists(char *filename) {
    struct stat buffer;   
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "%s: \e[31;1mfatal error\e[0m: no input file specified\n", argv[0]);
        fprintf(stderr, "usage: %s [input] [out?]\n", argv[0]);
        return 1;
    }

    if(!file_exists(argv[1])) {
        fprintf(stderr, "input file `%s` does not exist.\n", argv[1]);
        return 1;
    }


    FILE* file = fopen(argv[1], "r");

    if(argc == 3)
        assemble(file, argv[2]);
    else
        assemble(file, "a.out");
    return 0;
}