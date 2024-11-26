#include <common/common.h>

#	define MAX_FACES 100
#	define FACE_SIZE 3
#	define MAX_WHEELS 10

typedef struct {
	char faces[MAX_WHEELS][MAX_FACES][FACE_SIZE];
	int faceCount[MAX_WHEELS];
	int wheelSpeeds[MAX_WHEELS];
	int wheelCount;
} Data;

typedef struct {
	int pulls;
	int position[MAX_WHEELS];

	unsigned long long result;
} MemoKey;

#define MAX_MEMOIZATION_COUNT 100000
MemoKey memoization[MAX_MEMOIZATION_COUNT];
static int memoizationCount = 0;

static short charCount[256];
static bool maxMode = true;

const unsigned long long runMachine(const int remainingPulls, const Data* const data, const int wheelPositions[MAX_WHEELS]) {
	if (remainingPulls <= 0) return 0;

	// Memoization.
	MemoKey memoKey = {
		.pulls = remainingPulls,
		.result = 0,
	};
	memcpy(memoKey.position, wheelPositions, sizeof(*memoKey.position) * data->wheelCount);

	for (int i = 0; i < memoizationCount; i++) {
		if (memoization[i].pulls == memoKey.pulls &&
			memcmp(memoization[i].position, memoKey.position, sizeof(*memoKey.position) * data->wheelCount) == 0) {
			return memoization[i].result;
		}
	}

	int localWheelPositions[MAX_WHEELS];
	unsigned long long totalByteCoins = maxMode ? 0 : -1;
	for (int leftState = 0; leftState <= 2; leftState++) {
		for (int i = 0; i < data->wheelCount; i++)
			localWheelPositions[i] = wheelPositions[i];

		for (int i = 0; i < data->wheelCount; i++) {
			// Left pull.
			switch (leftState) {
				case 1: localWheelPositions[i] = (localWheelPositions[i] + 1) % data->faceCount[i]; break;
				case 2: localWheelPositions[i] = (localWheelPositions[i] + (data->faceCount[i] - 1)) % data->faceCount[i]; break;
			}
			// Right pull.
			localWheelPositions[i] = (localWheelPositions[i] + data->wheelSpeeds[i]) % data->faceCount[i];
		}

		memset(charCount, 0, sizeof(charCount));
		for (int i = 0; i < data->wheelCount; i++) {
			charCount[data->faces[i][localWheelPositions[i]][0]]++;
			charCount[data->faces[i][localWheelPositions[i]][2]]++;
		}

		unsigned long long localByteCoins = 0;
		for (int i = 0; i < sizeof(charCount) / sizeof(*charCount); i++)
			if (charCount[i] > 2)
				localByteCoins += (unsigned long long)(charCount[i] - 2);

		const unsigned long long result =
			localByteCoins +
			runMachine(remainingPulls - 1, data, localWheelPositions);

		if (maxMode) {
			if (totalByteCoins < result)
				totalByteCoins = result;
		} else {
			if (totalByteCoins > result)
				totalByteCoins = result;
		}
	}

	memoKey.result = totalByteCoins;
	if (memoizationCount < MAX_MEMOIZATION_COUNT) {
		memcpy(&memoization[memoizationCount++], &memoKey, sizeof(MemoKey));
	} else {
		printf("maxed out memoization.\n");
	}

	return totalByteCoins;
}

int main() {
	printf("\n");

	FILE* file = fopen("part3.txt", "rb");
	if (file == nullptr) return 1;

	Data data;
	for (int i = 0; i < MAX_WHEELS; i++) data.faceCount[i] = 0;
	for (int i = 0; i < MAX_WHEELS; i++) data.wheelSpeeds[i] = 0;
	data.wheelCount = 0;

	{
		char c;
		while (!feof(file)) {
			c = fgetc(file);

			if (c == ',' || c == '\n') {
				data.wheelCount++;
				if (c == '\n') break;
				continue;
			}

			data.wheelSpeeds[data.wheelCount] *= 10;
			data.wheelSpeeds[data.wheelCount] += c - '0';
		}

		fgetc(file);

		int currentWheel = 0;
		while (!feof(file)) {
			char face[FACE_SIZE];
			for (int i = 0; i < FACE_SIZE; i++)
				face[i] = fgetc(file);
			if (face[0] == '\n') {
				face[0] = face[1];
				face[1] = face[2];
				face[2] = fgetc(file);
			}
			fgetc(file);

			if (face[0] != ' ' &&
				face[1] != ' ' &&
				face[2] != ' ') {
				for (int i = 0; i < FACE_SIZE; i++)
					data.faces[currentWheel][data.faceCount[currentWheel]][i] = face[i];
				data.faceCount[currentWheel]++;
			}

			currentWheel += 1;
			currentWheel %= data.wheelCount;
		}
	}

	// Optimize offsets.
	for (int i = 0; i < data.wheelCount; i++)
		data.wheelSpeeds[i] %= data.faceCount[i];

	const unsigned long long targetPulls = 256;

	int wheelPositions[MAX_WHEELS];
	for (int i = 0; i < MAX_WHEELS; i++) wheelPositions[i] = 0;

	maxMode = true;
	const unsigned long long maxByteCoins = runMachine(targetPulls, &data, wheelPositions);
	memoizationCount = 0;
	maxMode = false;
	const unsigned long long minByteCoins = runMachine(targetPulls, &data, wheelPositions);

	printf("%llu %llu Max min ByteCoins.\n", maxByteCoins, minByteCoins);

	fclose(file);
}