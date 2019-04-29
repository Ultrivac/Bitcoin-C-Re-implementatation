#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "sha256.h"
#include "bitcoin.h"
#include <stdlib.h>

//NOT Machine independent function: machine needs to be little-endian
//takes the nonce converts it to BYTE array
//puts the BYTE array to little endian 
//replaces the pointer to the nonce with this neww nonce
void incrementNonce(BYTE* bitString, unsigned int Nonce){
    char *c = (char *)&Nonce;
    for(int i = 0; i < 4; i++) bitString[i] = c[i];
}

/*
Preconditions: all BLOCK fields are non NULL, little endian and Nonce is starts at 0x00000000
INPUT: BLOCK struct with fields in little endian unsigned char (BYTE) arrays
if the header hash complies to the difficulty then the header hash is returned 
else NULL is returned 
Modifies the BLOCK struct e.g. modifies nonce and *Tree struct for when the nonce
overflows
*/
BYTE *HashCash(BLOCK *block, int diffBytes){
    //before while loop we calculate unpacked difficulty target from the packed
    //target value
    //BYTE targetDifficulty[32];
    unsigned int Nonce = 0;
    while(1){
        //concatinates the fields of the block header 
        BYTE concatinatedHeader[80];
        for(int i = 0; i < 4; i++) concatinatedHeader[i] = block->Version[i];
        for(int i = 4, j = 0; i < 36 && j < 32; i++, j++) concatinatedHeader[i] = block->hashPrevBlock[j];
        for(int i = 36, j = 0; i < 68 && j < 32; i++, j++) concatinatedHeader[i] = block->hashMerkleRoot[j];   
        for(int i = 68, j = 0; i < 72 && j < 4; i++, j++) concatinatedHeader[i] = block->Time[j];   
        for(int i = 72, j = 0; i < 76 && j < 4; i++, j++) concatinatedHeader[i] = block->Bits[j]; 
        for(int i = 76, j = 0; i < 80 && j < 4; i++, j++) concatinatedHeader[i] = block->Nonce[j]; //Nonce is initially 0x00000000

        //hash buffers
        BYTE initialHash[32]; 
        BYTE *finalHash = malloc(32*sizeof(BYTE));   

        //first hash of block header (concatinatedHeader)
        SHA256_CTX ctx;
        sha256_init(&ctx); //initialize hashing function
	    sha256_update(&ctx, concatinatedHeader, 80); //80 represents the size in bytes of the block header concatination
	    sha256_final(&ctx, initialHash);

        //second hash of hashed block header
        sha256_init(&ctx); 
	    sha256_update(&ctx, initialHash, 32);//32 is the hash byte size of the initial block header hash 
	    sha256_final(&ctx, finalHash);

        //check finalHash conforms to difficulty target
        //if true return the finalHash, since the BLOCK* block struct is already
        //changed via its pointers throughout this loop the caller uses the struct to 
        //determine the winning nonce
        printSHA256(finalHash, 32);
        for(int i = 0; i < diffBytes; i++){
            if(finalHash[i] != 0x00) break;
            if(i == diffBytes-1) return finalHash;
        }
        //increment nonce after difficulty is checked against the current difficulty target
        //if it fails to conform to the difficulty
        //The block Nonce field is changed to reflect the increment
        //if the nonce overflows the coinbase transaction in the merkle tree will
        //be changed and as a result the hashMerkleRoot would be rehashed and changed.
        Nonce++;
        printf("%u\n", Nonce);
        if(Nonce != 0) incrementNonce(block->Nonce, Nonce);
        else{
             //if overflows changed rehash merkle root with new coinbase transaction
        }
    }
    return NULL;    
}
/*
int main(void){

    BLOCK block;
    BYTE Version[] = {0x01, 0x00, 0x00, 0x00};
    BYTE hashPrevBlock[] = {0x81, 0xcd, 0x02, 0xab, 0x7e, 0x56, 0x9e, 0x8b, 0xcd, 0x93, 0x17, 0xe2,
                            0xfe, 0x99, 0xf2, 0xde, 0x44, 0xd4, 0x9a, 0xb2, 0xb8, 0x85, 0x1b, 0xa4,
                            0xa3, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE hashMerkleRoot[] = {0xe3, 0x20, 0xb6, 0xc2, 0xff, 0xfc, 0x8d, 0x75, 0x04, 0x23, 0xdb, 0x8b,
                            0x1e, 0xb9, 0x42, 0xae, 0x71, 0x0e, 0x95, 0x1e, 0xd7, 0x97, 0xf7, 0xaf,
                            0xfc, 0x88, 0x92, 0xb0, 0xf1, 0xfc, 0x12, 0x2b};

    BYTE Time[] = {0xc7, 0xf5, 0xd7, 0x4d};
    BYTE Bits[] = {0xf2, 0xb9, 0x44, 0x1a};
    BYTE Nonce[] = {0x00, 0x00, 0x00, 0x00}; //final nonce is 0x42, 0xa1, 0x46, 0x95

    BYTE *finalHash;
    BYTE targetDifficulty[32] = {0x1d, 0xbd, 0x98, 0x1f, 0xe6, 0x98, 0x57, 0x76, 0xb6, 0x44, 0xb1, 
                                0x73, 0xa4, 0xd0, 0x38, 0x5d, 0xdc, 0x1a, 0xa2, 0xa8, 0x29, 0x68, 
                                0x8d, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //placing fields in block struct
    block.Version = Version;
    block.hashPrevBlock = hashPrevBlock;
    block.hashMerkleRoot = hashMerkleRoot;
    block.Time = Time;
    block.Bits = Bits;
    block.Nonce = Nonce;


    finalHash = HashCash(&block, targetDifficulty);
    printSHA256(finalHash, 32);
    printSHA256(block.Nonce, 4);

    return 0;
}
*/

















