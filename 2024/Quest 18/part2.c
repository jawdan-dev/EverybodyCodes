#include <common/common.h>
#include <common/point.h>

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;

#	define MAP_SIZE 256
	char map[MAP_SIZE][MAP_SIZE];

	int width, height;
	{
		int x = 0, y = 0;
		while (!feof(file)) {
			const char c = fgetc(file);
			if (c == '\n' || c == EOF) {
				width = x;
				x = 0;
				y++;
				continue;
			}
			map[x++][y] = c;
		}
		height = y;
	}

	int palmTreesRemaining = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == 'P')
				palmTreesRemaining++;

	typedef struct {
		Point pos;
		int time;
	} Node;

#	define NODE_MAX 1000
	Node nodes[NODE_MAX];
	int nodeCount = 0;

	for (int i = 0; i < width && entry.x == -1; i++) {
		if (map[i][0] == '.')
			nodes[nodeCount++] = (Node) { .pos = {.x = i, .y = 0 }, .time = 0 };
		if (map[i][height - 1] == '.')
			nodes[nodeCount++] = (Node) { .pos = {.x = i, .y = height - 1}, .time = 0 };
	}
	for (int i = 0; i < height && entry.x == -1; i++) {
		if (map[0][i] == '.')
			nodes[nodeCount++] = (Node) { .pos = {.x = 0, .y = i }, .time = 0 };
		if (map[width - 1][i] == '.')
			nodes[nodeCount++] = (Node) { .pos = {.x = width - 1, .y = i }, .time = 0 };
	}

	const Point checks[] = {
		{.x = +1, .y = +0 },
		{.x = -1, .y = +0 },
		{.x = +0, .y = +1 },
		{.x = +0, .y = -1 },
	};

	int lastNodeTime = 0;
	while (nodeCount > 0 && palmTreesRemaining > 0) {
		int bestIndex = 0;
		for (int i = 1; i < nodeCount; i++)
			if (nodes[bestIndex].time > nodes[i].time)
				bestIndex = i;

		const Node node = nodes[bestIndex];
		for (int i = bestIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		const int checkTime = node.time + 1;
		for (int i = 0; i < ARRAY_LENGTH(checks); i++) {
			const Point checkPoint = Point_add(&node.pos, &checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= width ||
				checkPoint.y < 0 || checkPoint.y >= height)
				continue;

			const char c = map[checkPoint.x][checkPoint.y];
			if (c != '.' && c != 'P') continue;

			if (c == 'P') {
				palmTreesRemaining--;
				if (lastNodeTime < checkTime)
					lastNodeTime = checkTime;
			}

			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.time = checkTime,
			};
			map[checkPoint.x][checkPoint.y] = '~';
		}
	}

	printf("%i Minutes required.\n", lastNodeTime);

	fclose(file);
}