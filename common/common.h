#pragma once

#define nullptr (void*)NULL

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const int abs(const int v) {
	return v >= 0 ? v : -v;
}