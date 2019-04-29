
#ifndef BITCOIN_H
#define BITCOIN_H
#include <stdio.h>

typedef unsigned char BYTE;

typedef struct {
    BYTE *hashPrevBlock;
    BYTE *hashMerkleRoot;
    BYTE *Version;
    BYTE *Time;
    BYTE *Bits; //packed target
    BYTE *Nonce;

}BLOCK;


void toBigEndian(BYTE* bitString, int length);
    
void toLittleEndian(BYTE* bitString, int length);

void printSHA256(BYTE *buf, int size);

#endif
