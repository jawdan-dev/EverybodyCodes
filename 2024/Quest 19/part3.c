#include <common/common.h>
#include <common/point.h>

#define SEQUENCE_MAX 200
static char sequence[SEQUENCE_MAX];
int sequenceCount = 0;
#define MAP_SIZE_MAX 256
static char map[MAP_SIZE_MAX][MAP_SIZE_MAX];
#define REMAP_MAX 10
Point remapPoints[MAP_SIZE_MAX][MAP_SIZE_MAX];
static int width, height;

void rotate(const Point* pos, const bool left) {
	static const Point checks[] = {
		{.x = -1, .y = -1 },
		{.x = +0, .y = -1 },
		{.x = +1, .y = -1 },
		{.x = +1, .y = +0 },
		{.x = +1, .y = +1 },
		{.x = +0, .y = +1 },
		{.x = -1, .y = +1 },
		{.x = -1, .y = +0 },
	};

	if (left) {
		const Point temp = remapPoints[pos->x + checks[0].x][pos->y + checks[0].y];
		for (int i = 1; i < ARRAY_LENGTH(checks); i++) {
			const Point
				to = Point_add(pos, &checks[i - 1]),
				from = Point_add(pos, &checks[i]);
			remapPoints[to.x][to.y] = remapPoints[from.x][from.y];
		}
		remapPoints[pos->x + checks[ARRAY_LENGTH(checks) - 1].x][pos->y + checks[ARRAY_LENGTH(checks) - 1].y] = temp;
	} else {
		const Point temp = remapPoints[pos->x + checks[ARRAY_LENGTH(checks) - 1].x][pos->y + checks[ARRAY_LENGTH(checks) - 1].y];
		for (int i = ARRAY_LENGTH(checks) - 1; i > 0; i--) {
			const Point
				to = Point_add(pos, &checks[i]),
				from = Point_add(pos, &checks[i - 1]);
			remapPoints[to.x][to.y] = remapPoints[from.x][from.y];
		}
		remapPoints[pos->x + checks[0].x][pos->y + checks[0].y] = temp;
	}
}

const bool isSorted() {
	Point open = { .x = -1, .y = -1 }, close = { .x = -1, .y = -1 };

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			const char c = map[x][y];
			if (c == '>') open = (Point) { .x = x, .y = y };
			if (c == '<') close = (Point) { .x = x, .y = y };
		}
	}
	ASSERT(open.x == -1, "Failed to find open bracket.");
	ASSERT(open.y == -1, "Failed to find close bracket.");

	if (open.y != close.y) return false;
	if (open.x >= close.x) return false;

	int symbolCount = 0;
	for (int i = open.x + 1; i < close.x; i++) {
		if (map[i][open.y] == '.' ||
			map[i][open.y] == ';' ||
			map[i][open.y] == ':' ||
			map[i][open.y] == '!' ||
			map[i][open.y] == '+')
			symbolCount++;
	}
	if (symbolCount > 5) return false;

	printf("Word found: ");
	for (int i = open.x + 1; i < close.x; i++)
		printf("%c", map[i][open.y]);
	printf("\n");

	return false;
}

void setupRemap(const int targetRemapCount) {
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			remapPoints[x][y] = (Point) { .x = x, .y = y };

	char s;
	Point rotationPoint;
	const int xExtent = width - 1, yExtent = height - 1;
	for (int i = 0; i < targetRemapCount; i++) {
		int sequenceIndex = 0;
		for (int y = 1; y < yExtent; y++) {
			rotationPoint.y = y;
			for (int x = 1; x < xExtent; x++) {
				s = sequence[sequenceIndex];
				sequenceIndex = (sequenceIndex + 1) % sequenceCount;
				rotationPoint.x = x;
				rotate(&rotationPoint, s == 'L');
			}
		}
	}
}

static char tempMap[MAP_SIZE_MAX][MAP_SIZE_MAX];
void rotateRemapped() {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			const Point remapPoint = remapPoints[x][y];
			tempMap[x][y] = map[remapPoint.x][remapPoint.y];
		}
	}

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			map[x][y] = tempMap[x][y];
}

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;


	// Read sequence.

	while (!feof(file)) {
		const char c = fgetc(file);
		if (c == '\n') break;
		sequence[sequenceCount++] = c;
	}
	fgetc(file);

	// Read character map.
	{
		int x = 0, y = 0;
		while (!feof(file)) {
			const char c = fgetc(file);
			if (c == '\n' || c == EOF) {
				width = x;
				ASSERT(width > MAP_SIZE_MAX, "Map width exceeded.\n");
				x = 0;
				y++;
				continue;
			}
			map[x++][y] = c;
		}
		height = y;
		ASSERT(height > MAP_SIZE_MAX, "Map height exceeded.\n");
	}

	fclose(file);

	// Rotate for funzies.
	int target = 1048576000;

	const int intervals[] = { 10000, 100, 1 };
	for (int i = 0; i < ARRAY_LENGTH(intervals); i++) {
		const int interval = intervals[i];
		setupRemap(interval);
		while (target >= interval) {
			rotateRemapped();
			target -= interval;
		}
	}

	isSorted();
}