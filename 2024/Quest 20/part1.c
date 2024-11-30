#include <common/common.h>
#include <common/point.h>

typedef enum {
	Direction_Up, Direction_Right, Direction_Down, Direction_Left, Direction_Count
} Direction;
const Direction Direction_flip(const Direction direction) {
	return (direction + 2) % Direction_Count;
}

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;

#	define MAP_SIZE 32
	char map[MAP_SIZE][MAP_SIZE];
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			map[i][j] = '#';
	int width, height;
	{
		int x = 0, y = 0;
		char c;
		while (!feof(file)) {
			c = fgetc(file);
			if (c == '\n' || c == EOF) {
				width = x;
				ASSERT(width > MAP_SIZE, "Map width exceeded.\n");
				x = 0;
				y++;
				continue;
			}
			map[x++][y] = c;
		}
		height = y;
		ASSERT(height > MAP_SIZE, "Map height exceeded.\n");
	}
	fclose(file);

	Point startingPoint = { .x = -1, .y = -1 };
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (map[i][j] == 'S')
				startingPoint = (Point) { .x = i, .y = j };
	ASSERT(startingPoint.x == -1, "Failed to find starting point.\n");

#	define NODE_MAX 1000
	typedef struct {
		Point pos;
		int height, time;
		Direction direction;
		int heuristic;
	} Node;
	Node* nodes = malloc(sizeof(Node) * NODE_MAX);
	int nodeCount = 0;

	nodes[nodeCount++] = (Node) {
		.pos = startingPoint,
		.height = 1000,
		.time = 0,
		.direction = Direction_Down,
	};

#	define TIME_MAX 100
	int bestHeights[MAP_SIZE][MAP_SIZE][Direction_Count][TIME_MAX];
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			for (int d = 0; d < Direction_Count; d++)
				for (int t = 0; t < TIME_MAX; t++)
					bestHeights[i][j][d][t] = 0;

	int bestHeight = 0;
	while (nodeCount > 0) {
		int bestIndex = 0;
		for (int i = 1; i < nodeCount; i++)
			if (nodes[bestIndex].heuristic < nodes[i].heuristic)
				bestIndex = i;
		const Node node = nodes[bestIndex];
		for (int i = bestIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		// printf("Node? %i, dir: %i, t: %i, d: %i, h: %i, p: { %i, %i }\n", bestIndex, node.direction, node.time, node.height, node.heuristic, node.pos.x, node.pos.y);
		if (node.time >= TIME_MAX) {
			if (bestHeight < node.height)
				bestHeight = node.height;
			continue;
		}

		const int checkTime = node.time + 1;
		static const Point checks[] = { {.x = +0, .y = -1 }, {.x = +1, .y = +0 }, {.x = +0, .y = +1 }, {.x = -1, .y = +0 }, };
		for (int checkIndex = 0; checkIndex < ARRAY_LENGTH(checks); checkIndex++) {
			if (checkIndex == Direction_flip(node.direction)) continue;
			const Point checkPoint = Point_add(&node.pos, &checks[checkIndex]);

			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height)
				continue;
			const char c = map[checkPoint.x][checkPoint.y];
			if (c == '#')
				continue;

			const int checkHeight = node.height + (c == '+') - ((c == '.') + ((c == '-') * 2));
			const Direction checkDirection = checkIndex;

			if (bestHeights[checkPoint.x][checkPoint.y][checkDirection][checkTime] >= checkHeight) continue;
			bestHeights[checkPoint.x][checkPoint.y][checkDirection][checkTime] = checkHeight;

			Node checkNode = {
				.pos = checkPoint,
				.time = checkTime,
				.height = checkHeight,
				.direction = checkDirection,
			};
			checkNode.heuristic = checkNode.height + checkNode.time;

			ASSERT(nodeCount >= NODE_MAX, "Node max exceeded.\n");
			memcpy(&nodes[nodeCount++], &checkNode, sizeof(Node));
		}
	}

	printf("Best height: %i\n", bestHeight);
}