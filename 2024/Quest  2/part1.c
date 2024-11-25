#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_WORDS 512
#	define MAX_WORD_LENGTH 512
	char words[MAX_WORDS][MAX_WORD_LENGTH];

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
		words[wordCount][currentWordLength++] = c;
	}

	// Ignore new line.
	fgetc(file);

	// Check for words.
	int totalWords = 0;
	char engraving[MAX_WORD_LENGTH];
	while (!feof(file)) {
		c = getc(file);
		if (c == ' ' || c == EOF) {
			engraving[currentWordLength] = '\0';

			for (int i = 0; i < wordCount; i++) {
				const int wordLen = strlen(words[i]);
				for (int j = wordLen; j <= currentWordLength; j++)
					if (strncmp(words[i], &engraving[j - wordLen], wordLen) == 0) {
						printf("%s from %s\n", words[i], &engraving[j - wordLen]);
						totalWords++;
					}
			}
			currentWordLength = 0;
			continue;
		}

		engraving[currentWordLength++] = c;
	}

	printf("%i Runic words.\n", totalWords);

	fclose(file);
}