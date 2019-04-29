#ifndef MERKLETREE_H
#define MERKLETREE_H
#include <stdio.h>

typedef struct MerkleTree MTree;

typedef struct Node Node;

struct MerkleTree{
    Node *root; //points to merkle root
    int numberOfTransactions;
};

MTree *initMerkleTree();
void createMerkleRoot(MTree *, BYTE *, int);
void getMerkleRootHashBigEndian(MTree *, BYTE *);
void getMerkleRootHashLittleEndian(MTree *, BYTE *);
void freeMerkleTree(MTree *);

#endif
