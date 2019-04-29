#include <stdio.h>
#include "bitcoin.h"

//assumes given string is little endian
//length is in bytes
void toBigEndian(BYTE* bitString, int length){
    for(int i = 1; i <= length/2; i++){
        BYTE temp = bitString[length - i];
        bitString[length - i] = bitString[i - 1];
        bitString[i - 1] = temp;
    } 
}

//assumes given string is big endian
//length is in bytes
void toLittleEndian(BYTE* bitString, int length){
    for(int i = 1; i <= length/2; i++){
        BYTE temp = bitString[i - 1];
        bitString[i - 1] = bitString[length - i];
        bitString[length - i] = temp;
    } 
}

void printSHA256(BYTE *buf, int size){
    for(int x = 0; x < size; x++){
        unsigned char mask1 = 0xf0;//create bit mask 11110000
        unsigned char mask2 = 0x0f;//create bit mask 00001111
        mask1 = mask1 & buf[x];//bitwise AND mas1 and the current char byte in the buffer
                               //this puts whatever these four end bits are into mask1
        mask1 = mask1 >> 4;//move these end 4 bits in the current char to the beginning
                            //of the char so that they can be later typecasted into 
                            //into an unsigned int so they can be converted to hex. 
        mask2 = mask2 & buf[x];//bitwise AND mask2 and the current char byte in the buffer
                               //this puts whatever these four beginning bits into mask1
        printf("%x%x", (unsigned int)mask1, (unsigned int)mask2);
   }	
   printf("\n"); 
}
