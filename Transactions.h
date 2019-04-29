#ifndef TRANS_DEF
#define TRANS_DEF


#include <stdio.h>
#include "bitcoin.h"

typedef struct TRANSACTION TRANSACTION;

typedef struct TXIN TXIN;

typedef struct TXOUT TXOUT;

TRANSACTION *initialiseTRANSACTION();

TXIN *initialiseTXIN(unsigned int scriptLength, BYTE *scriptSig, BYTE *previousTransaction, BYTE *sequenceNum);

TXOUT *initialiseTXOUT(unsigned int scriptLength, BYTE *scriptPubKey, BYTE *value);

void gettransaction(TRANSACTION *curr);

BYTE *getTransactionHash(TRANSACTION *t);

void hashTransaction(TRANSACTION *t);

void printTransactionHash(TRANSACTION *t);

void freeTRANSACTIONS(TRANSACTION *t);

#endif
