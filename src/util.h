#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

const char* DW_loadSourceFile(const char* filePath);

int64_t DW_currentTimeMillis();

void DW_sleepMillis(uint32_t ms);

#endif