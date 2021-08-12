#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

#ifndef SYNTHETIC_VERSION
#define SYNTHETIC_VERSION "nut"
#endif

bool file_exists(char *filename) {
    struct stat buffer;   
    return (stat(filename, &buffer) == 0);
}

void print_usage(char** argv) {
    fprintf(stderr, "usage: %s [image]\n", argv[0]);
}

int main(int argc, char** argv) {
    //printf("Synthetic Virtual Machine %s\n", SYNTHETIC_VERSION);
    
    if(argc == 1) {
        print_usage(argv);
        return 1;
    }

    if(!file_exists(argv[1])) {
        fprintf(stderr, "image file `%s` does not exist.\n", argv[1]);
        return 1;
    }

    FILE* file = fopen(argv[1], "rb");

    if(file == NULL) {
        fprintf(stderr, "error opening image file `%s`.\n", argv[1]);
        return 1;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(fileSize + 1);
    if(buffer == NULL) {
        fprintf(stderr, "error allocating memory to read `%s`.\n", argv[1]);
        return 1;
    }

    size_t bytesRead = fread(buffer, sizeof(uint8_t), fileSize, file);
    if(bytesRead < fileSize) {
        fprintf(stderr, "error reading file `%s`.\n", argv[1]);
        return 1;
    }

    buffer[bytesRead] = '\0';

    fclose(file);

    initVM();

    run(buffer);
    freeVM();
    return 0;
}