#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_COLUMNS 10
#	define MAX_ROWS 20
	int columns[MAX_COLUMNS][MAX_ROWS];
	int columnCount = 0;
	int columnCounts[MAX_COLUMNS];
	for (int i = 0; i < MAX_COLUMNS; i++)
		columnCounts[i] = 0;

	{
		int x = 0;
		while (!feof(file)) {
			const char c = fgetc(file);

			if (c == ' ') continue;
			if (c == '\n' || c == EOF) {
				if (columnCount < x)
					columnCount = x;
				x = 0;
				continue;
			}

			if ('0' <= c && c <= '9') {
				columns[x][columnCounts[x]] = c - '0';
				columnCounts[x]++;
			}
			x++;
		}
	}


	int marchColumn = 0;
	for (int n = 0; n < 10; n++)
	{
		const int marchValue = columns[marchColumn][0];
		for (int i = 1; i < columnCounts[marchColumn]; i++)
			columns[marchColumn][i - 1] = columns[marchColumn][i];
		columnCounts[marchColumn]--;

		const int nextColumn = (marchColumn + 1) % columnCount;
		const int nextMod = columnCounts[nextColumn] * 2;
		const int nextValue = marchValue % nextMod;

		const int nextInsertionIndex =
			(nextValue <= columnCounts[nextColumn]) ?
			nextValue - 1 :
			(columnCounts[nextColumn] - (nextValue - columnCounts[nextColumn])) + 1;

		columnCounts[nextColumn]++;
		for (int i = columnCounts[nextColumn]; i > nextInsertionIndex; i--)
			columns[nextColumn][i] = columns[nextColumn][i - 1];
		columns[nextColumn][nextInsertionIndex] = marchValue;

		marchColumn = (marchColumn + 1) % columnCount;
	}


	int maxRow = 0;
	for (int i = 0; i < columnCount; i++)
		if (maxRow < columnCounts[i])
			maxRow = columnCounts[i];
	for (int j = 0; j < maxRow; j++) {
		for (int i = 0; i < columnCount; i++) {
			if (columnCounts[i] > j) {
				printf("%i ", columns[i][j]);
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}

	fclose(file);
}