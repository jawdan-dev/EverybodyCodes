#include <common/common.h>

bool isFighter(char c) {
	return c == 'x' ? 0 : 1;
}
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

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

	int totalPotions = 0;

	char c1, c2, c3;
	while (!feof(file)) {
		c1 = fgetc(file);
		c2 = fgetc(file);
		c3 = fgetc(file);

		if (c1 == EOF || c2 == EOF || c3 == EOF) break;

		totalPotions += getPotionValue(c1) + getPotionValue(c2) + getPotionValue(c3);

		const int totalInFight = isFighter(c1) + isFighter(c2) + isFighter(c3);

		switch (totalInFight) {
			case 2: totalPotions += 2; break;
			case 3: totalPotions += 6; break;
		}
	}

	printf("%i Potions required.\n", totalPotions);

	fclose(file);
}