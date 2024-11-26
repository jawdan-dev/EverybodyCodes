#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_NUMBERS 128
	int numbers[MAX_NUMBERS];
	for (int i = 0; i < MAX_NUMBERS; i++)
		numbers[i] = 0;

	int numberCount = 0;
	char c;
	while (!feof(file)) {
		c = fgetc(file);

		if (c == '\n' || c == EOF) {
			numberCount++;
			continue;
		}

		numbers[numberCount] *= 10;
		numbers[numberCount] += c - '0';
	}

	// Get minimum as target.
	int min = numbers[0];
	for (int i = 1; i < numberCount; i++)
		if (min > numbers[i])
			min = numbers[i];

	// Strikin' time.
	int strikeCount = 0;
	for (int i = 0; i < numberCount; i++)
		strikeCount += numbers[i] - min;

	printf("%i Total strikes.\n", strikeCount);

	fclose(file);
}