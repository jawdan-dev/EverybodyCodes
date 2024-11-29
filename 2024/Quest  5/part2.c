#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_COLUMNS 6
#	define MAX_ROWS 100
	int columns[MAX_COLUMNS][MAX_ROWS];
	int columnCount = 0;
	int columnCounts[MAX_COLUMNS];
	for (int i = 0; i < MAX_COLUMNS; i++)
		columnCounts[i] = 0;

	{
		int x = 0;
		while (!feof(file)) {
			char c = fgetc(file);

			if ('0' <= c && c <= '9') {
				int value = 0;
				while ('0' <= c && c <= '9') {
					value *= 10;
					value += c - '0';
					c = fgetc(file);
				}
				columns[x][columnCounts[x]] = value;
				columnCounts[x]++;
				x++;
			}
			if (c == '\n' || c == EOF) {
				if (columnCount < x)
					columnCount = x;
				x = 0;
			}
		}
	}


#	define UNIQUE_SHOUT_MAX 1000000
	typedef struct {
		int value, count;
	} Shout;
	Shout* uniqueShouts = malloc(sizeof(Shout) * UNIQUE_SHOUT_MAX);
	int uniqueShoutCount = 0;

	const int shoutTarget = 2024;
	long long roundCount = 0;
	int marchColumn = 0;
	while (true) {
		ASSERT(columnCounts[marchColumn] <= 0, "No value in column to march.\n");
		const int marchValue = columns[marchColumn][0];
		for (int i = 1; i < columnCounts[marchColumn]; i++)
			columns[marchColumn][i - 1] = columns[marchColumn][i];
		columnCounts[marchColumn]--;

		const int nextColumn = (marchColumn + 1) % columnCount;
		int nextInsertionIndex = marchValue - 1;
		bool left = true;
		while (nextInsertionIndex >= columnCounts[nextColumn]) {
			nextInsertionIndex -= columnCounts[nextColumn];
			left = !left;
		}
		if (!left)
			nextInsertionIndex = columnCounts[nextColumn] - nextInsertionIndex;

		columnCounts[nextColumn]++;
		for (int i = columnCounts[nextColumn] - 1; i > nextInsertionIndex; i--)
			columns[nextColumn][i] = columns[nextColumn][i - 1];
		columns[nextColumn][nextInsertionIndex] = marchValue;

		int shoutValue = 0;
		for (int i = 0; i < columnCount; i++) {
			int columnValue = columns[i][0];
			while (columnValue > 0) {
				shoutValue *= 10;
				columnValue /= 10;
			}
			shoutValue += columns[i][0];
		}

		int shoutIndex = -1;
		for (int i = 0; i < uniqueShoutCount && shoutIndex == -1; i++)
			if (uniqueShouts[i].value == shoutValue)
				shoutIndex = i;
		if (shoutIndex == -1) {
			ASSERT(uniqueShoutCount >= UNIQUE_SHOUT_MAX, "Max unique shouts exceeded.\n");
			shoutIndex = uniqueShoutCount;
			uniqueShouts[uniqueShoutCount++] = (Shout) {
				.count = 0,
				.value = shoutValue,
			};
		}

		roundCount++;

		uniqueShouts[shoutIndex].count++;
		if (uniqueShouts[shoutIndex].count >= shoutTarget) {
			printf("Unique shout target reached with value of %i on round %lli.\n", shoutValue, roundCount);
			printf("Round x Shout = %lli\n", roundCount * (long long)shoutValue);
			break;
		}

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
				printf("   ");
			}
		}
		printf("\n");
	}

	fclose(file);
}