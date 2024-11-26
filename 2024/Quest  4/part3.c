#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_NUMBERS 512
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

	// Sort.
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < numberCount; i++) {
			if (numbers[i - 1] < numbers[i]) {
				const int temp = numbers[i - 1];
				numbers[i - 1] = numbers[i];
				numbers[i] = temp;
				sorted = false;
			}
		}
	}

	// Use median as target.
	const int target = numbers[numberCount / 2];

	// Strikin' time.
	int strikeCount = 0;
	for (int i = 0; i < numberCount; i++)
		if (numbers[i] > target)
			strikeCount += numbers[i] - target;
		else
			strikeCount += target - numbers[i];

	printf("%i Total strikes.\n", strikeCount);

	fclose(file);
}