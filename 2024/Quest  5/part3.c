#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
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
		long long value;
		int count;
	} Shout;
	Shout* uniqueShouts = malloc(sizeof(Shout) * UNIQUE_SHOUT_MAX);
	int uniqueShoutCount = 0;

	const int shoutTarget = 1000000 - 1;
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

		long long shoutValue = 0;
		for (int i = 0; i < columnCount; i++) {
			long long columnValue = columns[i][0];
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
			printf("Unique shout target reached with value of %lli on round %lli.\n", shoutValue, roundCount);
			break;
		}


		static int debugCounter = 0;
		if (++debugCounter > 212301) {
			debugCounter = 0;

			long long highest = 0;
			for (int i = 0; i < uniqueShoutCount; i++)
				if (highest < uniqueShouts[i].count)
					highest = uniqueShouts[i].count;
			printf("[%lli] %lli / %lli\r", roundCount, highest, shoutTarget);
		}

		marchColumn = (marchColumn + 1) % columnCount;
	}

	long long highest = 0;
	for (int i = 0; i < uniqueShoutCount; i++)
		if (highest < uniqueShouts[i].value)
			highest = uniqueShouts[i].value;
	printf("Highest: [%lli]\n", highest);

	fclose(file);
}