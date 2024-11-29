#include <common/common.h>

#define NAME_MAX 10
#define NAME_MAXB NAME_MAX + 1
#define NODE_MAX 100
#define UNKNOWN_PARENT "??????????"
typedef struct {
	char name[NAME_MAXB];
	char parent[NAME_MAXB];
	int depth;
	bool hasFruit;
} Node;
Node nodes[NODE_MAX];
int nodeCount = 0;

const int insertNode(const char name[NAME_MAXB], const char parent[NAME_MAXB], const int depth) {
	int index = -1;
	for (int i = 0; i < nodeCount && index == -1; i++)
		if (strcmp(name, nodes[i].name) == 0)
			index = i;

	if (index == -1) {
		ASSERT(nodeCount >= NODE_MAX, "Node max exceeded.\n");
		index = nodeCount++;
		for (int i = 0; i < NAME_MAXB; i++) {
			nodes[index].name[i] = name[i];
			nodes[index].parent[i] = parent[i];
		}
		nodes[index].hasFruit = false;
		nodes[index].depth = -1;
	}

	if (strcmp(nodes[index].parent, UNKNOWN_PARENT) == 0 && strcmp(parent, UNKNOWN_PARENT) != 0)
		for (int i = 0; i < NAME_MAXB; i++)
			nodes[index].parent[i] = parent[i];

	if (nodes[index].depth < 0 && depth >= 0)
		nodes[index].depth = depth;

	return index;
}
const int getNodeIndex(const char name[NAME_MAXB]) {
	int index = -1;
	for (int i = 0; i < nodeCount && index == -1; i++)
		if (strcmp(name, nodes[i].name) == 0)
			index = i;
	return index;
}

void printNodePath(const int index) {
	if (index == -1) return;
	const int parentIndex = getNodeIndex(nodes[index].parent);
	printNodePath(parentIndex);
	printf("%s", nodes[index].name);
}

int main() {
	printf("\n");

	FILE* file = fopen("part1.txt", "rb");
	if (file == nullptr) return 1;



	while (!feof(file)) {
		char c = fgetc(file);

		char nameBuffer[NAME_MAXB];
		int nameLength = 0;

		// Read root name.
		while (c != ':') {
			nameBuffer[nameLength++] = c;
			c = fgetc(file);
		}
		nameBuffer[nameLength] = '\0';

		// Insert root node.
		int depth = -1;
		if (strcmp(nameBuffer, "RR") == 0) depth = 0;
		const int rootIndex = insertNode(nameBuffer, UNKNOWN_PARENT, depth);
		if (depth == -1) depth = nodes[rootIndex].depth;
		if (depth != -1) depth++;

		// Try to insert into node list.
		while (c != '\n' && c != EOF) {
			char target[NAME_MAX];
			nameLength = 0;
			if (c == ':' || c == ',') c = fgetc(file);
			while (c != ',' && c != '\n' && c != EOF) {
				nameBuffer[nameLength++] = c;
				c = fgetc(file);
			}
			nameBuffer[nameLength] = '\0';

			if (strcmp(nameBuffer, "@") == 0) {
				nodes[rootIndex].hasFruit = true;
			} else {
				insertNode(nameBuffer, nodes[rootIndex].name, depth);
			}
		}
	}
	fclose(file);

	bool depthResolved = false;
	while (!depthResolved) {
		depthResolved = true;
		for (int i = 0; i < nodeCount; i++) {
			if (nodes[i].depth != -1) continue;

			const int parentIndex = getNodeIndex(nodes[i].parent);
			if (parentIndex == -1 || nodes[parentIndex].depth == -1) {
				depthResolved = false;
				continue;
			}

			nodes[i].depth = nodes[parentIndex].depth + 1;
		}
	}

	for (int i = 0; i < nodeCount; i++)
		printf("[%i] %s -> %s%c\n", nodes[i].depth, nodes[i].parent, nodes[i].name, nodes[i].hasFruit ? '*' : ' ');

	int maxDepth = 0;
	for (int i = 0; i < nodeCount; i++)
		if (maxDepth < nodes[i].depth)
			maxDepth = nodes[i].depth;
	int* fruitCounter = malloc(sizeof(int) * maxDepth + 1);
	for (int i = 0; i <= maxDepth; i++)
		fruitCounter[i] = 0;
	for (int i = 0; i < nodeCount; i++)
		if (nodes[i].hasFruit)
			fruitCounter[nodes[i].depth]++;

	int bestDepth = 0;
	for (int i = 1; i <= maxDepth; i++)
		if (fruitCounter[i] > 0 &&
			(fruitCounter[bestDepth] == 0 || fruitCounter[bestDepth] > fruitCounter[i]))
			bestDepth = i;

	printf("Best depth: %i\n", bestDepth);
	for (int i = 0; i < nodeCount; i++) {
		if (nodes[i].depth == bestDepth && nodes[i].hasFruit) {
			printNodePath(i);
			printf("@\n");
			break;
		}
	}
}