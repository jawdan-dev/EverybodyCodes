#include <common/common.h>
#include <common/point.h>

typedef struct {
	Point pos;
	int cost;
} Node;

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;


#	define MAP_SIZE 256
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
				x = 0;
				y++;
				continue;
			}

			map[x++][y] = c;
		}
		height = y;
	}

	const char target = 'H';
	Point startingPos = {
		.x = 0,
		.y = 0,
	};
	for (int i = 0; i < width; i++) {
		if (map[i][0] == '.') {
			startingPos.x = i;
			break;
		}
	}

#	define MAX_NODES 10000
	Node nodes[MAX_NODES];
	int nodeCount = 0;

	nodes[nodeCount++] = (Node) {
		.cost = 0,
		.pos = startingPos,
	};

	const Point checks[] = {
		{.x = +1, .y = +0 },
		{.x = -1, .y = +0 },
		{.x = +0, .y = +1 },
		{.x = +0, .y = -1 },
	};

	int travelCost = -1;
	while (nodeCount > 0) {
		int bestIndex = 0;
		for (int i = 1; i < nodeCount; i++)
			if (nodes[bestIndex].cost > nodes[i].cost)
				bestIndex = i;

		const Node node = nodes[bestIndex];
		for (int i = bestIndex + 1; i < nodeCount; i++)
			nodes[i - 1] = nodes[i];
		nodeCount--;

		if (map[node.pos.x][node.pos.y] == target) {
			travelCost = node.cost;
			break;
		}

		const int checkCost = node.cost + 1;
		for (int i = 0; i < ARRAY_LENGTH(checks); i++) {
			const Point checkPoint = Point_add(&node.pos, &checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height)
				continue;
			if (map[checkPoint.x][checkPoint.y] == '#')
				continue;

			bool duplicateFound = false;
			for (int n = 0; n < nodeCount && !duplicateFound; n++) {
				if (Point_equal(&nodes[n].pos, &checkPoint)) {
					if (checkCost < nodes[n].cost)
						nodes[n].cost = checkCost;
					duplicateFound = true;
				}
			}

			if (duplicateFound)
				continue;

			if (nodeCount > MAX_NODES) {
				printf("Max nodes exceeded.\n");
				continue;
			}

			nodes[nodeCount++] = (Node) {
				.pos = checkPoint,
				.cost = checkCost,
			};
		}
	}

	printf("Travel cost: %i\n", travelCost * 2);


	fclose(file);
}