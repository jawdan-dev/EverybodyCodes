#include <common/common.h>
#include <common/point.h>

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;

#	define STARS_MAX 1000
	Point* stars = malloc(sizeof(Point) * STARS_MAX);
	int starCount = 0;

	{
		int x = 0, y = 1;
		while (!feof(file)) {
			const char c = fgetc(file);

			x++;
			if (c == '\n' || c == EOF) {
				x = 0;
				y++;
				continue;
			}

			if (c != '*') continue;

			if (starCount >= STARS_MAX) {
				printf("Stars max exceeded.\n");
				return 1;
			}

			stars[starCount++] = (Point) {
				.x = x,
				.y = y,
			};
		}
	}

	int** starDistances = malloc(sizeof(int*) * STARS_MAX);
	for (int i = 0; i < STARS_MAX; i++)
		starDistances[i] = malloc(sizeof(int) * STARS_MAX);

	for (int i = 0; i < starCount; i++) {
		for (int j = i + 1; j < starCount; j++) {
			starDistances[i][j] = starDistances[j][i] = Point_distance(&stars[i], &stars[j]);
		}
	}

#	define PAIRS_MAX 1000
	Point* pairs = malloc(sizeof(Point) * PAIRS_MAX);
	int pairCount = 0;

	for (int i = 0; i < starCount; i++) {
		int closestIndex = i == 0 ? 1 : 0;
		for (int j = 0; j < starCount; j++) {
			if (i == j) continue;

			if (starDistances[i][closestIndex] > starDistances[i][j])
				closestIndex = j;
		}

		const Point pair = {
			.x = (i < closestIndex) ? i : closestIndex,
			.y = (i < closestIndex) ? closestIndex : i
		};

		bool found = false;
		for (int j = 0; j < pairCount && !found; j++)
			if (Point_equal(&pair, &pairs[j]))
				found = true;
		if (found) continue;

		if (pairCount >= PAIRS_MAX) {
			printf("Pair max exceeded.\n");
			return 1;
		}

		pairs[pairCount++] = pair;
	}

	printf("%i stars.\n", starCount);
	printf("%i pairs.\n", pairCount);
	for (int j = 0; j < pairCount; j++)
		printf("Pair [ %i %i ] x [ %i %i ]\n", stars[pairs[j].x].x, stars[pairs[j].x].y, stars[pairs[j].y].x, stars[pairs[j].y].y);

	// Setup groups.
	int* mainGroup = malloc(sizeof(int) * STARS_MAX), * subGroup = malloc(sizeof(int) * STARS_MAX);
	mainGroup[0] = 0;
	for (int i = 1; i < starCount; i++)
		subGroup[i - 1] = i;
	int mainGroupCount = 1, subGroupCount = starCount - mainGroupCount;

	while (subGroupCount > 0) {
		// Find best connection to add.
		Point bestPair = { .x = -1, .y = -1 };
		for (int i = 0; i < mainGroupCount; i++) {
			for (int j = 0; j < pairCount; j++) {
				if (pairs[j].x != mainGroup[i] && pairs[j].y != mainGroup[i]) continue;

				const int otherIndex = pairs[j].x == mainGroup[i] ? pairs[j].y : pairs[j].x;

				// Make sure the index isn't already in use.
				bool found = false;
				for (int k = 0; k < mainGroupCount && !found; k++)
					if (mainGroup[k] == otherIndex)
						found = true;
				if (found) continue;

				if (bestPair.x == -1 ||
					starDistances[bestPair.x][bestPair.y] > starDistances[mainGroup[i]][otherIndex]) {
					// Better pair found.
					bestPair.x = mainGroup[i];
					bestPair.y = otherIndex;
				}
			}
		}
		if (bestPair.x != -1) {
			mainGroup[mainGroupCount++] = bestPair.y;
			for (int k = 0; k < subGroupCount; k++) {
				if (subGroup[k] == bestPair.y) {
					for (int l = k + 1; l < subGroupCount; l++)
						subGroup[l - 1] = subGroup[l];
					subGroupCount--;
					break;
				}
			}
			continue;
		}

		Point shortest = { .x = mainGroup[0], .y = subGroup[0] };
		for (int i = 0; i < mainGroupCount; i++)
			for (int j = 0; j < subGroupCount; j++)
				if (starDistances[shortest.x][shortest.y] > starDistances[mainGroup[i]][subGroup[j]])
					shortest = (Point) { .x = mainGroup[i], .y = subGroup[j] };

		if (shortest.x > shortest.y) {
			const int temp = shortest.x;
			shortest.x = shortest.y;
			shortest.y = temp;
		}

		if (pairCount >= PAIRS_MAX) {
			printf("Group pair max exceeded.\n");
			return 1;
		}

		pairs[pairCount++] = shortest;
	}

	printf("%i pairs.\n", pairCount);
	for (int j = 0; j < pairCount; j++)
		printf("Pair [ %i %i ] x [ %i %i ]\n", stars[pairs[j].x].x, stars[pairs[j].x].y, stars[pairs[j].y].x, stars[pairs[j].y].y);

	printf("const points = [ ");
	for (int j = 0; j < pairCount; j++)
		printf("{ x: %i, y: %i }, { x: %i, y: %i }, ", stars[pairs[j].x].x, stars[pairs[j].x].y, stars[pairs[j].y].x, stars[pairs[j].y].y);
	printf("];\n");

	printf("const order = [");
	for (int j = 0; j < mainGroupCount; j++)
		printf("{ x: %i, y: %i }, ", stars[mainGroup[j]].x, stars[mainGroup[j]].y);
	printf("];\n");


	int constellationSum = 0;
	for (int i = 0; i < pairCount; i++)
		constellationSum += starDistances[pairs[i].x][pairs[i].y];
	printf("Constellation Sum: %i\n", constellationSum);
	printf("Constellation Answer: %i\n", constellationSum + starCount);


	fclose(file);
}