#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_MAP_SIZE 256
	char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
	char mapCopy[MAX_MAP_SIZE][MAX_MAP_SIZE];

	int width, height;
	{
		// Read in map.
		int x = 0, y = 0;
		char c;
		while (!feof(file)) {
			c = fgetc(file);
			if (c == '\n' || c == EOF) {
				width = x;
				x = 0;
				y++;
				if (c == EOF) break;
				continue;
			}
			map[x++][y] = c;
		}
		height = y;
	}

	int total = 0;

	int inLayer = 1;
	while (inLayer > 0) {
		// Count remaining blocks.
		inLayer = 0;
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				if (map[x][y] == '#')
					inLayer++;
		total += inLayer;

		// Remove border.
		for (int x = 0; x < width; x++) {
			mapCopy[x][0] = '.';
			mapCopy[x][height - 1] = '.';
		}
		for (int y = 0; y < height; y++) {
			mapCopy[0][y] = '.';
			mapCopy[width - 1][y] = '.';
		}

		// Remove blocks with direct neighbours.
		for (int x = 1; x < width - 1; x++) {
			for (int y = 1; y < height - 1; y++) {
				mapCopy[x][y] = map[x][y];
				if (map[x][y] != '#') continue;

				if (map[x + 1][y - 0] != '.' &&
					map[x - 1][y - 0] != '.' &&
					map[x + 0][y + 1] != '.' &&
					map[x + 0][y - 1] != '.' &&
					map[x + 1][y - 1] != '.' &&
					map[x - 1][y - 1] != '.' &&
					map[x + 1][y + 1] != '.' &&
					map[x - 1][y + 1] != '.') continue;

				mapCopy[x][y] = '.';
			}
		}
		memcpy(map, mapCopy, sizeof(map));

		printf("removed : %i\n", inLayer);
	}


	printf("%i Blocks removed.\n", total);

	fclose(file);
}