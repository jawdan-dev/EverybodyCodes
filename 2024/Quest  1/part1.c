#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;

	int totalPotions = 0;
	char c;
	while (!feof(file)) {

		switch (c = fgetc(file)) {
			case 'A': break;
			case 'B': totalPotions += 1; break;
			case 'C': totalPotions += 3; break;
		}
	}

	printf("%i Potions required.\n", totalPotions);

	fclose(file);
}