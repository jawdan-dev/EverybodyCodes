#include <common/common.h>

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
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

	const int targetPulls = 100;
	for (int i = 0; i < wheelCount; i++)
		for (int j = 0; j < targetPulls; j++)
			wheelPositions[i] = (wheelPositions[i] + wheelSpeeds[i]) % faceCount[i];

	for (int i = 0; i < wheelCount; i++)
		printf("%c%c%c ", faces[i][wheelPositions[i]][0], faces[i][wheelPositions[i]][1], faces[i][wheelPositions[i]][2]);

	fclose(file);
}