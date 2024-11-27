#include <common/common.h>

// Honestly, i just manually optimized the map.
// 358 -> 79 targets
// Basically i just reduced the search space p much.
// This one wasn't too fun for me.

#define MAP_SIZE 256
#define MAX_UNIQUE_FLOWERS 15
#define TARGETS_MAX 128

typedef struct {
	int x, y;
} Point;
typedef struct {
	Point pos;
	int cost;
} Node;

int abs(const int v) {
	return v >= 0 ? v : -v;
}

const bool point_equal(const Point* p1, const Point* p2) {
	return p1->x == p2->x && p1->y == p2->y;
}
const Point point_add(const Point* p1, const Point* p2) {
	return (Point) {
		.x = p1->x + p2->x,
			.y = p1->y + p2->y,
	};
}
const int point_distance(const Point* p1, const Point* p2) {
	int cx = p1->x - p2->x;
	int cy = p1->y - p2->y;
	return abs(cx) + abs(cy);
}

static char map[MAP_SIZE][MAP_SIZE];
static int width, height;
static char flowersIDS[MAX_UNIQUE_FLOWERS];
static int flowerCount = 0;
static Point targets[TARGETS_MAX];
static int targetCost[TARGETS_MAX + 1][MAP_SIZE][MAP_SIZE];
static int flowerCost[MAX_UNIQUE_FLOWERS][MAP_SIZE][MAP_SIZE];
static int targetCount = 0;
static Point startingPoint;


#define MAX_NODES 10000
static Node nodes[MAX_NODES];
void expandPathCost(const Point* origin, int output[MAP_SIZE][MAP_SIZE]) {
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			output[i][j] = -1;

	int nodeCount = 0;

	nodes[nodeCount] = (Node) {
		.cost = 0,
		.pos = *origin,
	};
	nodeCount++;

	static const Point checks[] = {
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

		if (output[node.pos.x][node.pos.y] != -1 && output[node.pos.x][node.pos.y] < node.cost) continue;
		output[node.pos.x][node.pos.y] = node.cost;

		const int checkStartDistance = node.cost + 1;
		for (int i = 0; i < ARRAY_LENGTH(checks); i++) {
			const Point checkPoint = point_add(&node.pos, &checks[i]);

			if (checkPoint.x < 0 || checkPoint.x >= width || checkPoint.y < 0 || checkPoint.y >= height)
				continue;
			const char c = map[checkPoint.x][checkPoint.y];
			if (c == '#' || c == '~') continue;

			bool duplicateFound = false;
			for (int n = 0; n < nodeCount && !duplicateFound; n++) {
				if (point_equal(&nodes[n].pos, &checkPoint)) {
					if (checkStartDistance < nodes[n].cost)
						nodes[n].cost = checkStartDistance;
					duplicateFound = true;
				}
			}

			if (duplicateFound)
				continue;

			if (nodeCount > MAX_NODES) {
				printf("Max nodes exceeded.\n");
				continue;
			}

			nodes[nodeCount] = (Node) {
				.pos = checkPoint,
				.cost = checkStartDistance,
			};
			nodeCount++;
		}
	}
}

const int getStartLength(const Point* start) {
	return targetCost[TARGETS_MAX][start->x][start->y];
}
const int getTargetLength(const Point* start, const int targetIndex) {
	return targetCost[targetIndex][start->x][start->y];
}


typedef struct {
	Point from;
	bool foundFlowers[MAX_UNIQUE_FLOWERS];
} MemoizationKey;

typedef int MemoizationResult;

#include <common/memoization.h>

const int getMinFlowerDist(const Point* from, const bool foundFlowers[MAX_UNIQUE_FLOWERS]) {
	// Check if we're at the final stage.
	int remainingFlowers = 0;
	for (int i = 0; i < flowerCount; i++)
		if (!foundFlowers[i])
			remainingFlowers++;
	// Handle exit.
	if (remainingFlowers <= 0)
		return getStartLength(from);

	// Memoize.
	MemoizationKey key = { .from = *from, };
	for (int i = 0; i < flowerCount; i++)
		key.foundFlowers[i] = foundFlowers[i];
	for (int i = flowerCount; i < MAX_UNIQUE_FLOWERS; i++)
		key.foundFlowers[i] = false;

	MemoizationResult result;
	if (Memoization_getResult(&key, &result))
		return result;

	// Go to flowers.
	bool localFlowers[MAX_UNIQUE_FLOWERS];
	for (int i = 0; i < flowerCount; i++)
		localFlowers[i] = foundFlowers[i];

	// Get best path length;
	int bestPathCost = -1;
	for (int t = 0; t < targetCount; t++) {
		const char c = map[targets[t].x][targets[t].y];

		int flowerIndex = -1;
		for (int i = 0; i < flowerCount; i++)
			if (flowersIDS[i] == c)
				flowerIndex = i;
		if (flowerIndex == -1 || localFlowers[flowerIndex]) continue;

		static int debugVal = 0;
		static int debugDepth = 0;
		if (++debugVal > 100000 || remainingFlowers > debugDepth) {
			debugVal = 0;
			debugDepth = remainingFlowers;
			printf("[%2.i] \t%2.i / %2.i \t|| \tMemoization Entries: %9.llu\n", remainingFlowers, t + 1, targetCount, memoizationCount);
		}

		localFlowers[flowerIndex] = true;
		const int dist =
			getTargetLength(from, t) +
			getMinFlowerDist(&targets[t], localFlowers);
		localFlowers[flowerIndex] = false;

		if (bestPathCost == -1 || bestPathCost > dist)
			bestPathCost = dist;
	}

	// Cache.
	Memoization_add(&key, &bestPathCost);

	return bestPathCost;
}


int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;


	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			map[i][j] = '#';

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
	printf("Map size: %i %i\n", width, height);
	if (width > MAP_SIZE) {
		printf("Max map width exceeded.\n");
		return 1;
	}
	if (height > MAP_SIZE) {
		printf("Max map height exceeded.\n");
		return 1;
	}

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			const char c = map[i][j];
			if ('A' <= c && c <= 'Z') {
				bool alreadyFound = false;
				for (int k = 0; k < flowerCount && !alreadyFound; k++)
					if (flowersIDS[k] == c)
						alreadyFound = true;

				if (alreadyFound) continue;

				if (flowerCount >= MAX_UNIQUE_FLOWERS) {
					printf("Flower max exceeded.\n");
					break;
				}
				flowersIDS[flowerCount++] = c;
			}
		}
	}
	printf("found flowers: %i\n", flowerCount);

	// Setup starting point.
	startingPoint = (Point) {
		.x = 0,
		.y = 0,
	};
	for (int i = 0; i < width; i++) {
		if (map[i][0] == '.') {
			startingPoint.x = i;
			break;
		}
	}

	// Generate targets.
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			const char c = map[i][j];
			if (!('A' <= c && c <= 'Z')) continue;

			if (targetCount >= TARGETS_MAX) {
				printf("Targets max exceeded.\n");
				break;
			}

			targets[targetCount++] = (Point) {
				.x = i,
				.y = j,
			};
		}
	}

	// Expand paths.
	printf("Expanding %i paths.\n", targetCount + 1);
	for (int i = 0; i < targetCount; i++)
		expandPathCost(&targets[i], targetCost[i]);
	expandPathCost(&startingPoint, targetCost[TARGETS_MAX]);
	printf("Paths expanded.\n");

	bool foundFlowers[MAX_UNIQUE_FLOWERS];
	for (int i = 0; i < flowerCount; i++)
		foundFlowers[i] = false;
	const int travelCost = getMinFlowerDist(&startingPoint, foundFlowers);
	printf("Travel cost: %i\n", travelCost);


	fclose(file);
}