#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "sha256.h"
#include "Transactions.h"
#include "bitcoin.h"
#include "HashCash.h"
#include "MerkleTreeADT.h"

int main(void){
    int transactionNum = 0;
    printf("Specify a number of transactions you want to add: ");
    scanf("%d", &transactionNum);

    //gets the details of transactions and hashes them, then puts their transaction ID hash into a string for the merkle tree
    
    BYTE transactionString[transactionNum*32];
    for(int i = 0; i < transactionNum; i++){
        TRANSACTION *t = initialiseTRANSACTION();
        gettransaction(t);
        memcpy(transactionString + i*32, getTransactionHash(t), 32);
        free(t);
    }
    MTree *Tree = initMerkleTree();
   	BYTE merkleHash[32];
    createMerkleRoot(Tree, transactionString, transactionNum);
   	getMerkleRootHashBigEndian(Tree, merkleHash);
    printf("\nMerkle Root Hash: ");
    printSHA256(merkleHash, 32);
    BLOCK block;
    BYTE Version[] = {0x01, 0x00, 0x00, 0x00};
    BYTE hashPrevBlock[] = {0x81, 0xcd, 0x02, 0xab, 0x7e, 0x56, 0x9e, 0x8b, 0xcd, 0x93, 0x17, 0xe2,
                            0xfe, 0x99, 0xf2, 0xde, 0x44, 0xd4, 0x9a, 0xb2, 0xb8, 0x85, 0x1b, 0xa4,
                            0xa3, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    BYTE Time[] = {0xc7, 0xf5, 0xd7, 0x4d};
    BYTE Bits[] = {0xf2, 0xb9, 0x44, 0x1a};
    BYTE Nonce[] = {0x00, 0x00, 0x00, 0x00}; 

    BYTE *finalHash;
    printf("Enter the byte difficulty (1 easiest, 4+ takes a while): ");
    int diffBytes = 0;
    scanf("%d", &diffBytes);
    //placing fields in block struct
    block.Version = Version;
    block.hashPrevBlock = hashPrevBlock;
    block.hashMerkleRoot = merkleHash;
    block.Time = Time;
    block.Bits = Bits;
    block.Nonce = Nonce;


    finalHash = HashCash(&block, diffBytes);
    printf("\nFound Hash!! This Block will have hash: ");
    printSHA256(finalHash, 32);
    printf("Nonce value: %d\n", *(unsigned int *)block.Nonce);
    freeMerkleTree(Tree);
    return 0;
}
