#pragma once

#define nullptr (void*)NULL

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))
#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)

#define ASSERT(condition, msg) \
	if (condition) { \
		printf(msg); \
		exit(1); \
	}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const int abs(const int v) {
	return v >= 0 ? v : -v;
}