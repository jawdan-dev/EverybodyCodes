#include <common/common.h>
#include <common/point.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
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

	typedef struct {
		Point pos;
		int time;
		int waterIndex;
	} Node;

#	define NODE_MAX 1000
	Node nodes[NODE_MAX];
	int nodeCount = 0;


#	define WATER_MAX 200
	int waterTotal[MAP_SIZE][MAP_SIZE];
	int*** waterTraversed = malloc(sizeof(int**) * width);

	int palmTreesCount = 0;
	for (int i = 0; i < width; i++) {
		waterTraversed[i] = malloc(sizeof(int*) * height);
		for (int j = 0; j < height; j++) {
			waterTotal[i][j] = 0;
			waterTraversed[i][j] = malloc(sizeof(int) * WATER_MAX);
			for (int k = 0; k < WATER_MAX; k++)
				waterTraversed[i][j][k] = -1;

			if (map[i][j] == 'P') {
				waterTraversed[i][j][palmTreesCount] = 0;
				nodes[nodeCount++] = (Node) {
					.pos = {.x = i, .y = j },
					.time = 0,
					.waterIndex = palmTreesCount++,
				};
				waterTotal[i][j]++;
				map[i][j] = '.';
			}
		}
	}

	const Point checks[] = {
		{.x = +1, .y = +0 },
		{.x = -1, .y = +0 },
		{.x = +0, .y = +1 },
		{.x = +0, .y = -1 },
	};


	while (nodeCount > 0) {
		int bestIndex = 0;
		for (int i = 1; i < nodeCount; i++)
			if (nodes[bestIndex].time > nodes[i].time)
				bestIndex = i;

		const Node node = nodes[bestIndex];
		for (int i = bestIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		const int checkTime = node.time + 1;
		const int checkWaterIndex = node.waterIndex;
		for (int i = 0; i < ARRAY_LENGTH(checks); i++) {
			const Point checkPoint = Point_add(&node.pos, &checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= width ||
				checkPoint.y < 0 || checkPoint.y >= height)
				continue;

			const char c = map[checkPoint.x][checkPoint.y];
			if (c != '.' || waterTraversed[checkPoint.x][checkPoint.y][checkWaterIndex] >= 0) continue;

			if (nodeCount >= NODE_MAX) {
				printf("Node max exceeded.\n");
				continue;
			}

			waterTraversed[checkPoint.x][checkPoint.y][checkWaterIndex] = checkTime;
			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.time = checkTime,
				.waterIndex = checkWaterIndex,
			};
			waterTotal[checkPoint.x][checkPoint.y]++;
		}
	}


	int bestTime = -1;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (waterTotal[i][j] < palmTreesCount) continue;

			int timeSum = 0;
			bool inPalmTree = false;
			for (int k = 0; k < palmTreesCount && !inPalmTree; k++) {
				timeSum += waterTraversed[i][j][k];
				if (waterTraversed[i][j][k] == 0)
					inPalmTree = true;
			}
			if (inPalmTree) continue;

			if (bestTime == -1 || bestTime > timeSum)
				bestTime = timeSum;
		}
	}

	printf("%i Minutes required.\n", bestTime);

	fclose(file);
}