#include "util.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>


const char* DW_loadSourceFile(const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    
    if (!file) {
        fprintf(stderr, "Error: Unable to open source file %s\n", filePath);
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    uint32_t fileSize = ftell(file);
    rewind(file);

    char* textBuf = malloc(fileSize + 1);
    if (!textBuf) {
        fprintf(stderr, "Error: Failed to malloc text buffer for source file %s\n", filePath);
        free(textBuf);
        fclose(file);
        return NULL;
    }

    // read all bytes into our buf
    size_t bytesRead = fread(textBuf, 1, fileSize, file);
    textBuf[fileSize] = '\0';

    if (bytesRead < fileSize) {
        fprintf(stderr, "Error: Failed reading source file %s bytesRead: %u fileSize: %u\n", filePath, bytesRead, fileSize);
        free(textBuf);
        fclose(file);
        return NULL;
    }

    // add our null terminator

    fclose(file);
    return textBuf;
}

// skidded from stackoverflow, requires gnu lib sys/time.h
int64_t DW_currentTimeMillis() {
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t)(time.tv_sec) * 1000;
    int64_t s2 = (time.tv_usec / 1000);
    return s1 + s2;
}

// Sleep function that supports compilation across platforms
#include <unistd.h>

void DW_sleepMillis(uint32_t ms) {
    usleep(ms * 1000); // usleep takes microseconds
}

float DW_lerp(float then, float now, float delta) {
    return then + (now - then) * delta;
}