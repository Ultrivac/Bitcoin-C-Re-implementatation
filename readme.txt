How Bitcoin Works

Each week I have conducted much research and as a result of this research, I provided code to further prove my understanding of the research I undertook.

An overview of this research project is to understand the technical working of the Bitcoin protocol and replicate the protocol in my own C implementation. By the end of my this semester I plan to have covered all features of the protocol as well as a working offline replica of my own version of the protocol (which is technically equivalent to Bitcoin). By the end I will also attempt to preform some attacks on the Bitcoin Block Chain and research into any other potential attacks.

This can replicate the hashing/ mining of blocks and test the output against real bitcoin blocks found in the blockchain. Below is the final product and contains all the files needed to run my smaller simpler bitcoin client. This client essentially asks for transaction details, creates transaction hash ids, creates a Merkle root from these transaction ID's and begins finding the block header by "mining" the block. After it finds the header it stops and displays the hash of the block you have found, it then exits. This client replicates some of the main cryptographic protocols of bitcoin which include Transaction hashes, Merkle Tree data integrity and a cryptographic proof of work algorithm for mining (HashCash algorithm). 

To compile, you will need to run the following command in the directory of the files below:

gcc -Wall -Werror bitcoin.c HashCash.c main.c MerkleTreeADT.c sha256.c Transaction.c -o bitcoinClient

to run:
./bitcoinClient

All files are my work excluding sha256.c and sha256.h which I used for the SHA256 algorithm from the following source: https://github.com/B-Con/crypto-algorithms/find/master

