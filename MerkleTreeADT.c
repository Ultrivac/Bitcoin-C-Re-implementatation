#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "sha256.h"
#include "MerkleTreeADT.h"
#include "bitcoin.h"

struct Node{
    Node *left;
    Node *right;
    Node *parent;//maintains a pointer to the parent node
    BYTE *hash;
};

static Node *initNode(BYTE *hash){
    Node *n = malloc(sizeof(Node));
    assert(n != NULL);
    n->hash = malloc(32*sizeof(BYTE));
    assert(n->hash != NULL);
    memcpy(n->hash, hash, 32);//copies hash to n->hash
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    return n;
}

MTree *initMerkleTree(){
    MTree *m = malloc(sizeof(MTree));
    assert(m != NULL);
    m->numberOfTransactions = 0;
    m->root = NULL; 
    return m;
}

//takes big endian 32 bit hashes
//reverses hash a1 and a2 to little-endian
//concatinates them
//hashes the concatination twice
//reverses the final hash to big-endian and returns this hash to caller
static void hash(BYTE *a1, BYTE *a2, BYTE *finalHash){
    toLittleEndian(a1, 32);
    toLittleEndian(a2, 32);

    //concatinate a1 and a2
    BYTE concatination[64];
    for(int x = 0, y = 0; x < 64; x++){
        if(x < 32) concatination[x] = a1[x];
        else{
            concatination[x] = a2[y];
            y++;
        }
    }

    BYTE initialHash[32]; 
  
    //Hashing
    SHA256_CTX ctx;
    sha256_init(&ctx); //initialize hashing function
	sha256_update(&ctx, concatination, 64); //64 is size of the concatination of a1 and a2
	sha256_final(&ctx, initialHash);

    //second hash of hashed block header
    sha256_init(&ctx); 
    sha256_update(&ctx, initialHash, 32);//32 is the hash byte size of the initial block header hash 
	sha256_final(&ctx, finalHash);

    //puts hash back into little endian
    toBigEndian(finalHash, 32);
}

//a recursive function which takes in an array of nodes, number of nodes in array and the MTree
//this function returns to its caller if mtree->root != NULL, if its NULL then it implies that
//the function has not finished producing a merkleroot.
static Node *buildMerkleTree(Node **nodeArray, int *nodeNum){
    if(*nodeNum == 1) return nodeArray[0]; //merkle root is found

    int nodeNumSentinal = 0;
    //if nodeNum is odd then duplicate last node and add to nodeArray
    if((*nodeNum)%2 == 1){
        nodeNumSentinal++;
    }
    
    
    if(nodeNumSentinal == 1){
        *nodeNum = (*nodeNum+1)/2;
    }else{
        *nodeNum = (*nodeNum)/2;
    }
    Node *temp[*nodeNum];

    int counter = 0;
    for(int x = 0; x < 2*(*nodeNum); x += 2){
        BYTE finalHash[32];

        if(nodeNumSentinal == 1 && x == ((2*(*nodeNum)) - 2)){
            hash(nodeArray[x]->hash, nodeArray[x]->hash, finalHash);
            Node *n = initNode(nodeArray[x]->hash);
            
            Node *parent = initNode(finalHash);
            parent->left = nodeArray[x];
            parent->right = n;

            //sets the nodes in the nodeArray to have their parent pointer point to the newly formed parent
            nodeArray[x]->parent = parent;
            n->parent = parent;
            //puts the new parent into a newly forming nodeArray which is currenlty named temp
            temp[counter] = parent;
        }else{

            hash(nodeArray[x]->hash, nodeArray[x+1]->hash, finalHash);
            //creates parent node and sets it's pointers to the two sibling nodes
            Node *parent = initNode(finalHash);
            parent->left = nodeArray[x];
            parent->right = nodeArray[x+1];

            //sets the nodes in the nodeArray to have their parent pointer point to the newly formed parent
            nodeArray[x]->parent = parent;
            nodeArray[x+1]->parent = parent;
            //puts the new parent into a newly forming nodeArray which is currenlty named temp
            temp[counter] = parent;
        }
        counter++;
    }

    return buildMerkleTree(temp, nodeNum);
}



//*transactions represents the concatinated transaction hashes, where first 32 bytes
//of of this bitstring holds the coinbase transaction hash.
//if reusing mtree pointer then you need to free tree before reusing pointer in this function
//assumes BYTE *transaction contains an array which is transactionNum*32 BYTEs long.
void createMerkleRoot(MTree *mtree, BYTE *transactions, int transactionNum){
    /*Sets up transactions if odd then duplicates last transaction and appends it to transaction*/
    BYTE temp[32*(transactionNum+1)];//the +1 is used in case odd number of transactions
    if(transactionNum % 2 == 1 && transactionNum != 1){
        transactionNum++; //adds 1 in preporation to create the first set or parent nodes
        memcpy(temp, transactions, 32*(transactionNum - 1));
         
        /*Copies last 32 bytes of transactions string into temp's last 32 empty bytes*/
        for(int i = ((transactionNum-1)*32), j = ((transactionNum-2)*32); i < (((transactionNum-1)*32) + 32) && j < ((transactionNum-1)*32); i++, j++)      {   
            temp[i] = transactions[j];
        }
        transactions = &temp[0];//changes transactions to point to the new larger transaction bitstring. CANNOT return this pointer NOW
        
    }else if (transactionNum == 1){
        //if transactionNum is one then the MerkleRoot the transaction hash
        Node *n = initNode(transactions);
        mtree->root = n;
        mtree->numberOfTransactions = 1;
        return;
    }
    
    /*Puts transaction Nodes in array to later be added to merkle tree*/
    Node *nodeArray[transactionNum];
    int counter = 0;
    for(int i = 0; i < transactionNum; i++){
        BYTE holder[32];
        //puts 32 bytes of a transaction from transactions into the holder
        for(int x = 0; x < 32; counter++, x++){
            holder[x] = transactions[counter];
        }
        Node *n = initNode(holder);
        nodeArray[i] = n;
    }
    
    /*Recursively build merkle tree*/
    mtree->numberOfTransactions = transactionNum;
    mtree->root = buildMerkleTree(nodeArray, &transactionNum);
}

//big-endian version of the merkle root put in string.
//string is 32 BYTEs long
void getMerkleRootHashBigEndian(MTree *mtree, BYTE *string){
    memcpy(string, mtree->root->hash, 32);//currently in big-endian by default
}

//little-endian version of the merkle root put in string.
//string is 32 BYTEs long
void getMerkleRootHashLittleEndian(MTree *mtree, BYTE *string){
    memcpy(string, mtree->root->hash, 32);
    toLittleEndian(string, 32);
}

static void freeMerkleTreeNodes(Node *root){
    if(root == NULL) return;
    if(root->left != NULL)freeMerkleTreeNodes(root->left);
    if(root->right != NULL)freeMerkleTreeNodes(root->right);
    free(root->hash);
    free(root);
}

void freeMerkleTree(MTree *mtree){
    freeMerkleTreeNodes(mtree->root);
    free(mtree);
}









