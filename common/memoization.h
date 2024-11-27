#pragma once
#include <common/common.h>

//typedef struct MemoizationKey;
//typedef struct MemoizationResult;

typedef struct {
	void* left, * right;

	MemoizationKey key;
	MemoizationResult result;
} MemoizationNode;

unsigned long long memoizationCount = 0;
MemoizationNode* memoizationTree = nullptr;

MemoizationNode** Memoization_findRaw(MemoizationNode** const root, const MemoizationKey* key) {
	if (*root == nullptr) return root;
	const int cmp = memcmp(&(*root)->key, key, sizeof(MemoizationKey));
	if (cmp < 0) return Memoization_findRaw((MemoizationNode**)&(*root)->left, key);
	if (cmp > 0) return Memoization_findRaw((MemoizationNode**)&(*root)->right, key);
	return root;
}

void Memoization_add(const MemoizationKey* const key, const MemoizationResult* const result) {
	MemoizationNode** nodePtr = Memoization_findRaw(&memoizationTree, key);

	if (*nodePtr != nullptr) return;
	*nodePtr = malloc(sizeof(MemoizationNode));

	MemoizationNode* const node = *nodePtr;
	memcpy(&node->key, key, sizeof(MemoizationKey));
	memcpy(&node->result, result, sizeof(MemoizationResult));
	node->left = nullptr;
	node->right = nullptr;

	// Statistics.
	memoizationCount++;
}

const bool Memoization_getResult(const MemoizationKey* const key, MemoizationResult* const result) {
	MemoizationNode** nodePtr = Memoization_findRaw(&memoizationTree, key);

	MemoizationNode* const node = *nodePtr;
	if (node == nullptr) return false;

	memcpy(result, &node->result, sizeof(MemoizationResult));
	return true;
}