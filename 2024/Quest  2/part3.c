#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_WORDS 128
#	define MAX_WORD_LENGTH 128
	char words[MAX_WORDS][MAX_WORD_LENGTH];
#	define MAX_LINES 512
#	define MAX_LINE_LENGTH 512
	char engravings[MAX_LINES][MAX_LINE_LENGTH];
	bool symbols[MAX_LINES][MAX_LINE_LENGTH];

	for (int i = 0; i < MAX_LINE_LENGTH; i++)
		for (int j = 0; j < MAX_LINES; j++)
			symbols[j][i] = false;

	// Ignore word prefix.
	while (fgetc(file) != ':');

	// Get words.
	int wordCount = 0, currentWordLength = 0;
	char c;
	while (true) {
		c = fgetc(file);
		if (c == ',' || c == '\n') {
			words[wordCount][currentWordLength] = '\0';
			currentWordLength = 0;
			wordCount++;
			if (c == '\n') break;
			continue;
		}
		if (wordCount > MAX_WORDS) return 1;
		words[wordCount][currentWordLength++] = c;
	}

	// Ignore new line.
	fgetc(file);

	// Get engravings.
	int x = 0, y = 0;
	while (!feof(file)) {
		c = getc(file);

		if (c == '\n' || c == EOF) {
			engravings[y][x] = '\0';
			y++;
			x = 0;
		} else {
			engravings[y][x++] = c;
		}
	}

	for (int j = 0; j < y; j++) {
		const int lineLength = strlen(engravings[j]);
		for (int i = 0; i < lineLength; i++) {
			for (int k = 0; k < wordCount; k++) {
				const int wordLen = strlen(words[k]);
				int n = 0, m = 0;

				// Check horizontal.
				for (int l = 0; l < wordLen; l++) {
					const char e = engravings[j][(i + l) % lineLength];
					if (words[k][l] == e) n++;
					if (words[k][(wordLen - l) - 1] == e) m++;
				}

				if (n == wordLen || m == wordLen)
					for (int l = 0; l < wordLen; l++)
						symbols[j][(i + l) % lineLength] = true;


				// Check vertical.

				// No wrapping.
				if (j + wordLen > y) continue;

				n = 0, m = 0;
				for (int l = 0; l < wordLen; l++) {
					const char e = engravings[(j + l) % y][i];
					if (words[k][l] == e) n++;
					if (words[k][(wordLen - l) - 1] == e) m++;
				}

				if (n == wordLen || m == wordLen)
					for (int l = 0; l < wordLen; l++)
						symbols[(j + l) % y][i] = true;
			}
		}
	}

	int totalRunicSymbols = 0;
	for (int j = 0; j < y; j++) {
		const int lineLength = strlen(engravings[j]);
		for (int i = 0; i < lineLength; i++) {
			if (symbols[j][i]) {
				printf("%c", engravings[j][i]);
				totalRunicSymbols++;
			} else {
				printf(" ");
			}
		}
		printf("\n");
	}

	printf("%i Runic symbols.\n", totalRunicSymbols);

	fclose(file);
}