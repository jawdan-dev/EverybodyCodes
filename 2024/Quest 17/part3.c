#include <common/common.h>
#include <common/point.h>

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

#	define STARS_MAX 10000
#	define STARS_DIST_MAX 6
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

#	define PAIRS_MAX STARS_MAX
	Point* pairs = malloc(sizeof(Point) * PAIRS_MAX);
	int pairCount = 0;

	for (int i = 0; i < starCount; i++) {
		int closestIndex = -1;
		for (int j = 0; j < starCount; j++) {
			if (i == j) continue;

			if (starDistances[i][j] >= STARS_DIST_MAX) continue;

			if (closestIndex == -1 || starDistances[i][closestIndex] > starDistances[i][j])
				closestIndex = j;
		}
		if (closestIndex == -1) continue;

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

	// Constellation storage.
#	define CONSTELLATIONS_MAX 200
	int* constellations[CONSTELLATIONS_MAX];
	int constellationCount = 0;
	int constellationCounts[CONSTELLATIONS_MAX];

	// Setup groups.
	int* mainGroup = malloc(sizeof(int) * STARS_MAX), * subGroup = malloc(sizeof(int) * STARS_MAX);
	for (int i = 0; i < starCount; i++) subGroup[i] = i;
	int mainGroupCount = 0, subGroupCount = starCount;

	while (subGroupCount > 0 || mainGroupCount > 0) {
		if (mainGroupCount <= 0) {
			mainGroup[0] = subGroup[0];
			for (int i = 1; i < starCount; i++)
				subGroup[i - 1] = subGroup[i];
			mainGroupCount++;
			subGroupCount--;
		}

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

				if (starDistances[mainGroup[i]][otherIndex] >= STARS_DIST_MAX) continue;

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

		Point shortest = { .x = -1, .y = -1 };
		for (int i = 0; i < mainGroupCount; i++)
			for (int j = 0; j < subGroupCount; j++)
				if (shortest.x == -1 ||
					starDistances[shortest.x][shortest.y] > starDistances[mainGroup[i]][subGroup[j]])
					shortest = (Point) { .x = mainGroup[i], .y = subGroup[j] };
		if (shortest.x > shortest.y) {
			const int temp = shortest.x;
			shortest.x = shortest.y;
			shortest.y = temp;
		}

		// Break off main group into a constellation.
		if (shortest.x == -1 || starDistances[shortest.x][shortest.y] >= STARS_DIST_MAX) {
			if (constellationCount >= CONSTELLATIONS_MAX) {
				printf("constellation max exceeded.\n");
				return 1;
			}

			constellations[constellationCount] = malloc(sizeof(int) * mainGroupCount);
			for (int i = 0; i < mainGroupCount; i++)
				constellations[constellationCount][i] = mainGroup[i];
			constellationCounts[constellationCount] = mainGroupCount;

			printf("constellation made of size %i.\n", mainGroupCount);

			constellationCount++;
			mainGroupCount = 0;


			continue;
		}

		if (pairCount >= PAIRS_MAX) {
			printf("Group pair max exceeded.\n");
			return 1;
		}

		pairs[pairCount++] = shortest;
	}


	int constellationSums[CONSTELLATIONS_MAX];
	for (int i = 0; i < constellationCount; i++) {
		constellationSums[i] = constellationCounts[i];
		for (int j = 0; j < pairCount; j++) {
			bool found = false;
			for (int k = 0; k < constellationCounts[i] && !found; k++)
				if (constellations[i][k] == pairs[j].x ||
					constellations[i][k] == pairs[j].y)
					found = true;
			if (found)
				constellationSums[i] += starDistances[pairs[j].x][pairs[j].y];
		}
	}

	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < constellationCount; i++) {
			if (constellationSums[i - 1] >= constellationSums[i]) continue;

			const int temp = constellationSums[i - 1];
			constellationSums[i - 1] = constellationSums[i];
			constellationSums[i] = temp;
			sorted = false;
		}
	}

	printf("Constellation scores: ");
	for (int i = 0; i < constellationCount; i++)
		printf("%i, ", constellationSums[i]);
	printf("\n");

	long long constellationProduct = 1;
	for (int i = 0; i < 3; i++)
		constellationProduct *= (long long)constellationSums[i];

	printf("Constellation Count: %i\n", constellationCount);
	printf("Top 3 Constellation Product: %lli\n", constellationProduct);

	fclose(file);
}