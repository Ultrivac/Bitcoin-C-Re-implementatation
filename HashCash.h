#ifndef HASH_DEF
#define HASH_DEF

#include <stdio.h>
#include "bitcoin.h"

void incrementNonce(BYTE* bitString, unsigned int Nonce);

BYTE *HashCash(BLOCK *block, int diffBytes);

#endif

