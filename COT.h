#ifndef COT_H
#define COT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "miracl.h"

#define K    1536
#define Pr    1
#define Ps    2
#define CidR  1
#define Sid   1
#define kk    128
#define kp   128
#define lse    128
#define ln    K
#define lm    292
#define le    lm+2
#define ls    ln+lse+lm
#define lnew   128

typedef struct PK
{
        big n;
	big g;
	big g1;
	big g2;
	big g3;
	big k;
}PK;

typedef struct PKS
{
        big n;
	big a1;
	big a2;
	big b;
	big c;
}PKS;

typedef struct SKS
{
        big p;
	big fi;
}SKS;

typedef struct PKC
{
        big h;
	big g;
	big n;
}PKC;

typedef struct PKP
{
        big q;
	big p;
	big g;
}PKP;

typedef struct idsCommit
{
        int P; 
        int cid;
}idsCommit;

typedef struct idsCOT1
{
        int PS; 
	int PR;
        int cid;
	int sid;
}idsCOT1;

typedef struct Com
{
        big u; 
        big e;
	big v;
}Com;

typedef struct OutputsCommit
{
	int Msg;
	int idsp;
	int idscid;
        big comu;
	big come;
	big comv;
	big s;
	big e;
	big v;
	big x;
	big rx;
	big commit;
}OutputsCommit;

typedef struct OutputsCOT1
{
	int Msg;
	int idsPS;
	int idsPR;
	int idscid;
	int idssid;
        big comu;
	big come;
	big comv;
}OutputsCOT1;

typedef struct OutputsCOT2
{
	int Msg;
	int idsPS;
	int idsPR;
	int idscid;
	int idssid;
        big u0; 
        big e0;
	big u1;
	big e1;
}OutputsCOT2;

miracl *mip;
struct PK pk;
struct PKS pks;
struct PKC pkc;
struct PKP pkp;
struct SKS sks;
big n2, r, alpha, x, rx, fi, m0, m1;
csprng rng, rng1;
int length;
char numberG[kp/8];
struct timeval t0, t1, t2, t3, t5, t6, t7, t8, PRG, tf, tk;
long long kk66, fattaneh,  onsender, onchooser, ofsender, ofchooser, cotofc, cotofs, cotonc, cotons, signofc, signofs, signonc, signons, bit1ofc, bit1ofs, bit1onc, bit1ons, bit2ofc, bit2ofs, bit2onc, bit2ons, psignofc, psignofs, psignonc, psignons, csignofc, csignofs, csignonc, csignons, ZKK1ofc, ZKK1ofs, ZKK1onc, ZKK1ons, ZKK2ofc, ZKK2ofs, ZKK2onc, ZKK2ons;
big PK0, PK1, khat;

void initialize(void);
void KeyGenerator(void);
void store_key(void);
void read_key(void);
struct OutputsCommit Commit(int, int);
struct OutputsCOT1 COT1(int, int, struct Com, big, big, big, big);
struct OutputsCOT2 COT2(struct OutputsCOT1, big, big);
big COT3(struct OutputsCOT2, int);
big Hash(big, big, big, int, int);
int ZKDLEQ(big, big, big, big);
int ZKCom(struct PK, struct Com, int, int, int);
big HashZK(big, big, big, big, big, big);
int ZKBit(big, int);
big HashBit(big, big, big, big, big);
int ZKSign(big, big, big, big, big, big, big);
big HashZKSign(big, big, big, big, big, big, big, big, big, big, big, big, big, big);
void printnum(big);
void KeyGeneratorPaternity();
void store_keyP();
void read_keyP();
big OT(int, big, big);
big HashP(big, int);
big XOR(int, big, big);
int ZK(big, int);
big HashZKK(big, big, big, big, big, big, big);
int ZK1(big, int, big, big);

big* OTextention(int, big*, big*, int*);
char* Gfunction(int, int, big);
char* Hfunction(int, int, int, char*);
big InittoBig(int, int*);
int* BigtoInt(int, big);
char* XORInt(int, big, int*);
char* XORByte(int, char*, char*);
char* InttoByte(int, int*);
int* BytetoInt(int, char*);

#endif
