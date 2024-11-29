#include <common/common.h>
#include <common/point.h>

#define SEQUENCE_MAX 1000
static char sequence[SEQUENCE_MAX];
#define MAP_SIZE_MAX 128
static char map[MAP_SIZE_MAX][MAP_SIZE_MAX];
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
		const char temp = map[pos->x + checks[0].x][pos->y + checks[0].y];
		for (int i = 1; i < ARRAY_LENGTH(checks); i++) {
			const Point
				to = Point_add(pos, &checks[i - 1]),
				from = Point_add(pos, &checks[i]);
			map[to.x][to.y] = map[from.x][from.y];
		}
		map[pos->x + checks[ARRAY_LENGTH(checks) - 1].x][pos->y + checks[ARRAY_LENGTH(checks) - 1].y] = temp;
	} else {
		const char temp = map[pos->x + checks[ARRAY_LENGTH(checks) - 1].x][pos->y + checks[ARRAY_LENGTH(checks) - 1].y];
		for (int i = ARRAY_LENGTH(checks) - 1; i > 0; i--) {
			const Point
				to = Point_add(pos, &checks[i]),
				from = Point_add(pos, &checks[i - 1]);
			map[to.x][to.y] = map[from.x][from.y];
		}
		map[pos->x + checks[0].x][pos->y + checks[0].y] = temp;
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

	for (int i = open.x + 1; i < close.x; i++)
		if (map[i][open.y] == '.')
			return false;

	printf("Word found: ");
	for (int i = open.x + 1; i < close.x; i++)
		printf("%c", map[i][open.y]);
	printf("\n");

	return true;
}

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;


	// Read sequence.
	int sequenceCount = 0;

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
				x = 0;
				y++;
				continue;
			}
			map[x++][y] = c;
		}
		height = y;
	}
	fclose(file);

	// Rotate for funzies.
	for (int i = 0; i < 100; i++) {
		int sequenceIndex = 0;
		for (int y = 1; y < height - 1; y++) {
			for (int x = 1; x < width - 1; x++) {
				const char s = sequence[sequenceIndex];
				sequenceIndex = (sequenceIndex + 1) % sequenceCount;

				Point rotationPoint = { .x = x, .y = y };
				rotate(&rotationPoint, s == 'L');
			}
		}
	}
	isSorted();
}