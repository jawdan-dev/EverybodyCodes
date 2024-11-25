#include <common/common.h>

int getPotionValue(char c) {
	switch (c) {
		case 'B': return 1;
		case 'C': return 3;
		case 'D': return 5;
	}
	return 0;
}

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;

	int totalPotions = 0;

	char c1, c2;
	while (!feof(file)) {
		c1 = fgetc(file);
		c2 = fgetc(file);

		if (c1 == EOF || c2 == EOF) break;

		totalPotions += getPotionValue(c1) + getPotionValue(c2);
		if (c1 != 'x' && c2 != 'x')
			totalPotions += 2;
	}

	printf("%i Potions required.\n", totalPotions);

	fclose(file);
}