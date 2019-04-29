#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "sha256.h"
#include "Transactions.h"
#include "bitcoin.h"

struct TXIN{
    BYTE previousTransactionHash[32];
    BYTE previous_n[4];
    BYTE sequenceNum[4]; 
    BYTE scriptLength[1]; //gives the length of the script in hexidecimal, 1-9 bytes in size
    BYTE *scriptSig; //the script which contains the hex format of the script with length scriptLength
};

struct TXOUT{
    BYTE value[8]; //the value of the output in satoshis
    BYTE scriptLength[1]; //length of scriptPubKey, ususally 1-9 bytes
    BYTE *scriptPubKey; //the scriptPubKey
};

struct TRANSACTION{
    BYTE Version[4];
    BYTE TransactionHash[32]; //the hash of the current transaction
    TXIN *Txin;
    BYTE TxinCounter; //keeps count of the number of Txin's or inputs associated with a transaction
    TXOUT *Txout;
    BYTE txoutCounter; //keeps track of the number of Txout's or outputs associated with a transaction
    BYTE lock_time[4];
};

TRANSACTION *initialiseTRANSACTION(){
    TRANSACTION *t = malloc(sizeof(TRANSACTION));
    assert(t != NULL);
    t->Version[0] = 0x01; t->Version[1] = 0x00; t->Version[2] = 0x00; t->Version[3] = 0x00;
    t->TxinCounter = 0x01;
    t->txoutCounter = 0x01;
    t->lock_time[0] = 0x00; t->lock_time[1] = 0x00; t->lock_time[2] = 0x00; t->lock_time[3] = 0x00;
    memset(t->TransactionHash, 0, 32);
    t->Txin = NULL;
    t->Txout = NULL;
    return t;
}

//initilises TXIN struct with the length of the script and the scriptSig itself
//sciptLength is 4 bytes and scriptSig is the byte stream of the provided scriptSig with size specified by the scriptLength byte stream
TXIN *initialiseTXIN(unsigned int scriptLength, BYTE *scriptSig, BYTE *previousTransaction, BYTE *sequenceNum){
    TXIN *in = malloc(sizeof(TXIN));
    assert(in != NULL);
    in->scriptSig = malloc(scriptLength*sizeof(BYTE));
    if(in->scriptSig == NULL){
        free(in);
        return NULL;    
    }
    memcpy(in->scriptLength, &scriptLength, 1); //copies the scriptLength value into the data structure field
    in->previous_n[0] = 0xff; in->previous_n[1] = 0xff; in->previous_n[2] = 0xff; in->previous_n[3] = 0xff;
    memcpy(in->scriptSig, scriptSig, scriptLength); //copies scriptSig to the TXIN data structure;
    memcpy(in->previousTransactionHash, previousTransaction, 32); 
    memcpy(in->sequenceNum, sequenceNum, 4); 
    return in;
}




//initilises TXOUT struct with the length of the script and the scriptPubKey itself
//sciptLength is 4 bytes and scriptPubKey is the byte stream of the provided scriptPubKey with size specified by the scriptLength byte stream
TXOUT *initialiseTXOUT(unsigned int scriptLength, BYTE *scriptPubKey, BYTE *value){
    TXOUT *out = malloc(sizeof(TXOUT));
    assert(out != NULL);
    out->scriptPubKey = malloc(scriptLength*sizeof(BYTE));
    assert(out->scriptPubKey != NULL);
    memcpy(out->scriptLength, &scriptLength, 1); //copies the scriptLength value into the data structure field
    memcpy(out->scriptPubKey, scriptPubKey, scriptLength); //copies scriptPubKey to the TXOUT data structure;
    memcpy(out->value, value, 8); //copies the value to the TXOUT data structure, the value fields length is known and is 8 bytes.
    return out;
}

//talks to stdin and recieves transaction details an modifies the provided transaction link list.
void gettransaction(TRANSACTION *curr){
       
        printf("Enter subsequent values in little-endian lowercase hexidecimal format\n");
        
        //gets input of a transaction
        BYTE previousTransactionHash[32]; memset(previousTransactionHash, 0, 32);
        BYTE scriptLength1[4]; memset(scriptLength1, 0, 4);
        BYTE scriptLength2[4]; memset(scriptLength2, 0, 4);
        BYTE sequenceNum[4]; memset(sequenceNum, 0, 4);

        printf("Enter previous transaction hash (64 hex characters): ");
        for(int i = 0; i < 8; i++){
            scanf("%x", (unsigned int*)previousTransactionHash+i);
            toLittleEndian(previousTransactionHash+(i*4), 4);
        }
     
        printf("Enter scriptLength (2 hex characters): ");
        scanf("%x", (unsigned int*)scriptLength1);

        BYTE scriptSig[(*(unsigned int *)scriptLength1/4 +1)*4];
        memset(scriptSig, 0, (*(unsigned int *)scriptLength1/4 +1)*4);
        printf("Enter scriptSig: ");
        for(int i = 0; i < (*(unsigned int *)scriptLength1/4 + 1); i++){
            scanf("%x", (unsigned int*)scriptSig + i);
            toLittleEndian(scriptSig+(i*4), 4);
        }
        
        printf("Enter sequenceNum (8 hex characters): ");
        scanf("%x", (unsigned int*)sequenceNum);
        curr->Txin = initialiseTXIN(*(unsigned int *)scriptLength1, scriptSig, previousTransactionHash, sequenceNum);
        //gets output of a transaction

        BYTE value[8]; memset(value, 0, 8);
        printf("Enter satoshi value of output (16 hex characters): ");
        for(int i = 0; i < 2; i++){ 
            scanf("%x", (unsigned int*)value + i);
            toLittleEndian(value+(i*4), 4);
        }

        printf("Enter scriptLength (2 hex characters): ");
        scanf("%x", (unsigned int*)scriptLength2);

        BYTE scriptPubKey[(*(unsigned int *)scriptLength2/4 +1)*4];
        memset(scriptPubKey, 0, (*(unsigned int *)scriptLength2/4 +1)*4);
        printf("Enter scriptPubKeys: ");
        for(int i = 0; i < (*(unsigned int *)scriptLength2/4 +1); i++){
            scanf("%x", (unsigned int*)scriptPubKey+i);
            toLittleEndian(scriptPubKey+(i*4), 4);
        }
        
        curr->Txout = initialiseTXOUT(*(unsigned int *)scriptLength2, scriptPubKey, value);

        hashTransaction(curr);
    


}

void hashTransaction(TRANSACTION *t){
    unsigned int txoutLength = *(unsigned int *)(t->Txout->scriptLength);
    unsigned int txinLength = *(unsigned int *)(t->Txin->scriptLength);
    int bitStringLength = 60 + txoutLength + txinLength;
    printf("\nRAW transaction:\n");
    printf("bitstring Length: %u\n", bitStringLength);
    BYTE bitString[bitStringLength];
    memset(bitString, 0, bitStringLength);
    memcpy(bitString, t->Version, 4);
    memcpy((bitString + 4), &(t->TxinCounter), 1);
    memcpy((bitString + 5), t->Txin->previousTransactionHash, 32);
    memcpy((bitString + 37), t->Txin->previous_n, 4);
    memcpy((bitString + 41), t->Txin->scriptLength, 1);
    memcpy((bitString + 42), t->Txin->scriptSig, txinLength);
    memcpy((bitString + 42 + txinLength), t->Txin->sequenceNum, 4);
    memcpy((bitString + 46 + txinLength), &(t->txoutCounter), 1);
    memcpy((bitString + 47 + txinLength), &(t->Txout->value), 8);
    memcpy((bitString + 55 + txinLength), t->Txout->scriptLength, 1);
    memcpy((bitString + 56 + txinLength), t->Txout->scriptPubKey, txoutLength);
    memcpy((bitString + 56 + txinLength + txoutLength), t->lock_time, 4);
    printSHA256(bitString, bitStringLength);
    BYTE initialHash[32];  
    //Hashing
    SHA256_CTX ctx;
    sha256_init(&ctx); //initialize hashing function
	sha256_update(&ctx, bitString, bitStringLength); //64 is size of the concatination of a1 and a2
	sha256_final(&ctx, initialHash);

    //second hash of hashed block header
    sha256_init(&ctx); 
    sha256_update(&ctx, initialHash, 32);//32 is the hash byte size of the initial block header hash 
	sha256_final(&ctx, t->TransactionHash);

    //puts hash back into little endian
    toBigEndian(t->TransactionHash, 32);
}

BYTE *getTransactionHash(TRANSACTION *t){
    return t->TransactionHash;
}


void printTransactionHash(TRANSACTION *t){
    printSHA256(t->TransactionHash, 32);
}

void freeTRANSACTIONS(TRANSACTION *t){
    free(t->Txin->scriptSig);
    free(t->Txin);
    free(t->Txout->scriptPubKey);
    free(t->Txout);
    free(t);
}











