#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part2.txt", "rb");
	if (file == nullptr) return 1;

#	define MAX_FACES 100
#	define FACE_SIZE 3
#	define MAX_WHEELS 10
	char faces[MAX_WHEELS][MAX_FACES][FACE_SIZE];
	int faceCount[MAX_WHEELS];
	for (int i = 0; i < MAX_WHEELS; i++)
		faceCount[i] = 0;

	int wheelSpeeds[MAX_WHEELS];
	for (int i = 0; i < MAX_WHEELS; i++)
		wheelSpeeds[i] = 0;

	int wheelCount = 0;
	{
		char c;
		while (!feof(file)) {
			c = fgetc(file);

			if (c == ',' || c == '\n') {
				wheelCount++;
				if (c == '\n') break;
				continue;
			}

			wheelSpeeds[wheelCount] *= 10;
			wheelSpeeds[wheelCount] += c - '0';
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
					faces[currentWheel][faceCount[currentWheel]][i] = face[i];
				faceCount[currentWheel]++;
			}

			currentWheel += 1;
			currentWheel %= wheelCount;
		}
	}

	int wheelPositions[MAX_WHEELS];
	for (int i = 0; i < MAX_WHEELS; i++)
		wheelPositions[i] = 0;

	// Optimize offsets.
	for (int i = 0; i < wheelCount; i++)
		wheelSpeeds[i] %= faceCount[i];

	const unsigned long long targetPulls = 202420242024;

	unsigned long long totalByteCoins = 0;

	short charCount[256];
	unsigned long long totalBasePull = 0;
	for (unsigned long long j = 0; j < targetPulls; j++) {
		memset(charCount, 0, sizeof(charCount));

		int totalPosition = 0;
		for (int i = 0; i < wheelCount; i++) {
			wheelPositions[i] = (wheelPositions[i] + wheelSpeeds[i]) % faceCount[i];
			totalPosition += wheelPositions[i];
		}

		for (int i = 0; i < wheelCount; i++) {
			charCount[faces[i][wheelPositions[i]][0]]++;
			charCount[faces[i][wheelPositions[i]][2]]++;
		}

		for (int i = 0; i < 256; i++)
			if (charCount[i] > 2)
				totalByteCoins += (unsigned long long)(charCount[i] - 2);


		if (totalPosition == 0) {
			totalBasePull = j + 1;
			break;
		}
	}

	if (totalBasePull != 0) {
		const unsigned long long pullBaseCount = targetPulls / totalBasePull;
		const unsigned long long pullRemainder = targetPulls % totalBasePull;

		totalByteCoins *= pullBaseCount;

		// Duplicate code for a sleepy boi.
		for (unsigned long long j = 0; j < pullRemainder; j++) {
			memset(charCount, 0, sizeof(charCount));

			for (int i = 0; i < wheelCount; i++)
				wheelPositions[i] = (wheelPositions[i] + wheelSpeeds[i]) % faceCount[i];

			for (int i = 0; i < wheelCount; i++) {
				charCount[faces[i][wheelPositions[i]][0]]++;
				charCount[faces[i][wheelPositions[i]][2]]++;
			}

			for (int i = 0; i < 256; i++)
				if (charCount[i] > 2)
					totalByteCoins += (unsigned long long)(charCount[i] - 2);
		}
	}

	printf("Base pulls: %llu\n", totalBasePull);

	printf("%llu Total ByteCoins.\n", totalByteCoins);

	fclose(file);
}