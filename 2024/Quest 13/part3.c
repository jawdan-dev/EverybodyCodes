#include <common/common.h>
#include <common/point.h>

typedef struct {
	Point pos;
	int time;
} Node;

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

#	define MAP_SIZE 256
	char map[MAP_SIZE][MAP_SIZE];
	int bestTimes[MAP_SIZE][MAP_SIZE];

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

#	define NODES_MAX 1000
	Node* nodes = malloc(sizeof(Node) * NODES_MAX);
	int nodeCount = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			bestTimes[i][j] = -1;

			if (map[i][j] == 'E') {
				nodes[nodeCount++] = (Node) {
					.pos = {.x = i, .y = j },
					.time = 0,
				};
				bestTimes[i][j] = 0;
				map[i][j] = '0';
			}
		}
	}

	const Point checks[] = {
		{.x = +1, .y = +0 },
		{.x = -1, .y = +0 },
		{.x = +0, .y = +1 },
		{.x = +0, .y = -1 },
	};

	int bestTime = 0;
	while (nodeCount > 0) {
		int bestIndex = 0;
		for (int i = 1; i < nodeCount; i++)
			if (nodes[bestIndex].time > nodes[i].time)
				bestIndex = i;

		const Node node = nodes[bestIndex];
		for (int i = bestIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		if (map[node.pos.x][node.pos.y] == 'S') {
			bestTime = bestTimes[node.pos.x][node.pos.y];
			break;
		}

		const int nodeHeight = map[node.pos.x][node.pos.y] - '0';
		for (int i = 0; i < ARRAY_LENGTH(checks); i++) {
			const Point checkPoint = Point_add(&node.pos, &checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= width ||
				checkPoint.y < 0 || checkPoint.y >= height)
				continue;

			if (map[checkPoint.x][checkPoint.y] == '#')
				continue;

			const char c = map[checkPoint.x][checkPoint.y];
			const int checkHeight = c == 'S' ? 0 : c - '0';
			const int checkTime = node.time + 1 +
				min(
					abs(nodeHeight - checkHeight),
					min(
						abs(nodeHeight + (10 - checkHeight)),
						abs((10 - nodeHeight) + checkHeight)
					)
				);

			if (bestTimes[checkPoint.x][checkPoint.y] != -1 &&
				bestTimes[checkPoint.x][checkPoint.y] < checkTime) continue;
			bestTimes[checkPoint.x][checkPoint.y] = checkTime;

			const Node nodeInfo = {
				.pos = checkPoint,
				.time = checkTime,
			};

			bool found = false;
			for (int j = 0; j < nodeCount && !found; j++) {
				const Node* node = &nodes[j];
				if (!Point_equal(&node->pos, &checkPoint)) continue;

				if (nodeInfo.time < node->time)
					nodes[j] = nodeInfo;

				found = true;
			}
			if (found) continue;

			ASSERT(nodeCount > NODES_MAX, "Node max exceeded.\n");
			nodes[nodeCount++] = nodeInfo;
		}
	}

	printf("Best time: %i\n", bestTime);

	fclose(file);
}