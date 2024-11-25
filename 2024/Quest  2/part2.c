#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
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
	int totalRunicSymbols = 0;
	char engraving[MAX_WORD_LENGTH];
	bool isEngravedSymbol[MAX_WORD_LENGTH];
	while (!feof(file)) {
		c = getc(file);
		if (c == ' ' || c == '\n' || c == EOF) {
			engraving[currentWordLength] = '\0';

			for (int i = 0; i < currentWordLength; i++)
				isEngravedSymbol[i] = false;

			for (int i = 0; i < wordCount; i++) {
				const int wordLen = strlen(words[i]);
				for (int j = wordLen; j <= currentWordLength; j++) {
					bool same = true;
					for (int k = 0; k < wordLen && same; k++)
						if (words[i][k] != engraving[(j - wordLen) + k])
							same = false;

					if (!same) {
						same = true;
						for (int k = 0; k < wordLen && same; k++)
							if (words[i][(wordLen - k) - 1] != engraving[(j - wordLen) + k])
								same = false;
					}

					if (same)
						for (int k = 0; k < wordLen && same; k++)
							isEngravedSymbol[(j - wordLen) + k] = true;
				}
			}

			for (int i = 0; i < currentWordLength; i++)
				if (isEngravedSymbol[i])
					totalRunicSymbols++;

			currentWordLength = 0;
			continue;
		}

		engraving[currentWordLength++] = c;
	}

	printf("%i Runic symbols.\n", totalRunicSymbols);

	fclose(file);
}