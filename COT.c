#include "COT.h"

void initialize(void)
{
        mip = mirsys(3*K, 2);
        mip->NTRY=100;
	length = ((2*K+1)/8)+1;
        char number[length];
        int i;
        long randnum;  
        struct timeval tp;
        gettimeofday(&tp, NULL);

        //srandomdev();
        srandom((unsigned int)tp.tv_usec);
        for (i = 0; i < 50; i++) {
                randnum = random();
                number[i*4] = (randnum & 0xFF000000) >> 24;
                number[i*4+1] = (randnum & 0xFF0000) >> 16;
                number[i*4+2] = (randnum & 0xFF00) >> 8;
                number[i*4+3] = randnum & 0xFF;
        }
        gettimeofday(&tp, NULL);
        strong_init(&rng, K/5 , number, (mr_unsign32)tp.tv_usec);
	gettimeofday(&PRG, NULL);

        return;
}

// key generation
void KeyGenerator()
{
        big p, q, np, pp, qp, gp, x1, x2, x3; 
     
        BOOL found = FALSE;
        BOOL found1 = FALSE;
        long int count = 0;

        //generating p' and q', p and q
        big number2;
        number2 = mirvar(2);
        pp = mirvar(0);
        p = mirvar(0);
        while (!found1){
                found = FALSE;
                while (!found){
                        strong_bigdig(&rng, K/2, 2, pp);
                        found = isprime(pp);
                        count++;
                }
                multiply(pp, number2, p);
                incr(p, 1, p);
                found1 = isprime(p);
        }
        printf("number of trials for p': %ld\n", count);
 	
	sks.p = mirvar(0);    	
	sks.p = p;

        found = FALSE;
        found1 = FALSE;
        qp = mirvar(0);
        q = mirvar(0);
        count = 0;
        while (!found1){
                found = FALSE;
                while (!found){
                        strong_bigdig(&rng, K/2, 2, qp);
                        found = isprime(qp);
                        count++;
                }
                multiply(qp, number2, q);
                incr(q, 1, q);
                found1 = isprime(q);
        }
        printf("number of trials for q': %ld\n", count);
     
        //computing np
        np = mirvar(0);
        multiply(pp, qp, np);

	// computing fi
	big res1, res2;
	res1 = mirvar(0);
	res2 = mirvar(0);
	sks.fi = mirvar(0);
	decr(p, 1, res1);
	decr(q, 1, res2);
	multiply(res1, res2, sks.fi);
		
        //computing n
        pk.n = mirvar(0);
        multiply(p, q, pk.n);
		
	// computing n2
	n2 = mirvar(0);
	multiply(pk.n, pk.n, n2);
	
	// computing g
	gp = mirvar(0);
	strong_bigrand(&rng, n2, gp);
	big temp1;
	temp1 = mirvar(0);
	add(pk.n, pk.n, temp1);
	pk.g = mirvar(0);
	powmod(gp, temp1, n2, pk.g);
		
	// computing g1, g2, and g3
	big temp2;
	temp2 = n2;
	sftbit(n2, -2, temp2);

	x1 = mirvar(0);
	strong_bigrand(&rng, temp2, x1);
	pk.g1 = mirvar(0);
	powmod(pk.g, x1, n2, pk.g1);
		
	x2 = mirvar(0);
	strong_bigrand(&rng, temp2, x2);
	pk.g2 = mirvar(0);
	powmod(pk.g, x2, n2, pk.g2);
		
	x3 = mirvar(0);
	strong_bigrand(&rng, temp2, x3);
	pk.g3 = mirvar(0);
	powmod(pk.g, x3, n2, pk.g3);
	
        //computing k
        pk.k = mirvar(0);
	strong_bigdig(&rng, K/2, 2, pk.k);

	// computing alpha
	alpha = mirvar(0);
	incr(pk.n, 1, alpha);

	// public key of signiture
	pks.n = mirvar(0);
	pks.n = pk.n;

	pks.a1 = mirvar(0);
	strong_bigrand(&rng, pks.n, pks.a1);
	big two;
	two = mirvar(2);
	powmod(pks.a1, two, pks.n, pks.a1);

	pks.a2 = mirvar(0);
	strong_bigrand(&rng, pks.n, pks.a2);
	powmod(pks.a2, two, pks.n, pks.a2);

	pks.b = mirvar(0);
	strong_bigrand(&rng, pks.n, pks.b);
	powmod(pks.b, two, pks.n, pks.b);

	pks.c = mirvar(0);
	strong_bigrand(&rng, pks.n, pks.c);
	powmod(pks.c, two, pks.n, pks.c);

	// public key of commitment
	pkc.n = mirvar(0);
	pkc.n = pk.n;

	pkc.h = mirvar(0);
	strong_bigrand(&rng, pkc.n, pkc.h);
	powmod(pkc.h, two, pkc.n, pkc.h);

	big random;
	random = mirvar(0);
	strong_bigrand(&rng, pkc.n, random);
	pkc.g = mirvar(0);
	powmod(pkc.h, random, pkc.n, pkc.g);
	
        return;
}

// store key on the file
void store_key()
{
        FILE *fp;
   
        fp = fopen("Public.txt", "w");
        mip->IOBASE = 16;
	cotnum(pk.n, fp);
        cotnum(pk.g, fp);
        cotnum(pk.g1, fp);
	cotnum(pk.g2, fp);
	cotnum(pk.g3, fp);
	cotnum(pk.k, fp);
	cotnum(pks.n, fp);
        cotnum(pks.a1, fp);
        cotnum(pks.a2, fp);
	cotnum(pks.b, fp);
	cotnum(pks.c, fp);
	cotnum(pkc.h, fp);
	cotnum(pkc.g, fp);
	cotnum(pkc.n, fp);
        fclose(fp);

        FILE *fp1;
        fp1 = fopen("Secret.txt", "w");
        mip->IOBASE = 16;
	cotnum(sks.p, fp1);
	cotnum(sks.fi, fp1);
        fclose(fp1);

        return;
}

// read key from the file
void read_key() 
{
        FILE *fp;
        fp = fopen("Public.txt", "r");
        mip->IOBASE = 16;
        pk.n = mirvar(0);
	pk.g = mirvar(0);
	pk.g1 = mirvar(0);
	pk.g2 = mirvar(0);
	pk.g3 = mirvar(0);
	pk.k = mirvar(0);
        pks.n = mirvar(0);
	pks.a1 = mirvar(0);
	pks.a2 = mirvar(0);
	pks.b = mirvar(0);
	pks.c = mirvar(0);
	pkc.h = mirvar(0);
	pkc.g = mirvar(0);
	pkc.n = mirvar(0);
		
        cinnum(pk.n, fp);
	cinnum(pk.g, fp);
	cinnum(pk.g1, fp);
	cinnum(pk.g2, fp);
	cinnum(pk.g3, fp);
	cinnum(pk.k, fp);
        cinnum(pks.n, fp);
	cinnum(pks.a1, fp);
	cinnum(pks.a2, fp);
	cinnum(pks.b, fp);
	cinnum(pks.c, fp);
	cinnum(pkc.h, fp);
	cinnum(pkc.g, fp);
	cinnum(pkc.n, fp);

        FILE *fp1;
        fp1 = fopen("Secret.txt", "r");
        mip->IOBASE = 16;
        sks.p = mirvar(0);
	sks.fi = mirvar(0);
		
        cinnum(sks.p, fp1);
	cinnum(sks.fi, fp1);
       
        // computing n2
        n2 = mirvar(0);
        multiply(pk.n, pk.n, n2);
		
	// computing alpha
	alpha = mirvar(0);
	incr(pk.n, 1, alpha);
	
        return;
}

// Commit
struct OutputsCommit Commit(int data, int data1)
{
	struct OutputsCommit result;
	result.Msg = 0;
	result.idsp = Pr;
	result.idscid = CidR;
	result.comu = mirvar(0);
	result.come = mirvar(0);
	result.comv = mirvar(0);
	result.e = mirvar(0);
	result.s = mirvar(0);
	result.v = mirvar(0);
	result.commit = mirvar(0);
	result.x = mirvar(data);
	result.rx = mirvar(0);
gettimeofday(&t0, 0);		
	big temp1;
	temp1 = mirvar(0);
	sftbit(pk.n, -2, temp1);
	r = mirvar(0);
	strong_bigrand(&rng, temp1, r);
	powmod(pk.g, r, n2, result.comu);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp2;
	temp2 = mirvar(data);
	powmod(alpha, temp2, n2, temp2);
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp3;
	temp3 = mirvar(0);
        powmod(pk.g1, r, n2, temp3);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
        mad(temp3, temp2, temp2, n2, n2, result.come);
		
	big temp4;
	temp4 = mirvar(0);
	temp4 = Hash(pk.k, result.comu, result.come, result.idsp, result.idscid);
		
	big temp5;
	temp5 = mirvar(0);
	powmod(pk.g3, temp4, n2, temp5);
        mad(temp5, pk.g2, pk.g2, n2, n2, temp5);
	powmod(temp5, r, n2, result.comv);
		
	big temp6;
	temp6 = mirvar(0);
	sftbit(n2, -1, temp6);
		
	if (mr_compare(result.comv, temp6) == 1)
		subtract(n2, result.comv, result.comv);
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// producing signiture
gettimeofday(&t0, 0);
	BOOL found = FALSE;
	result.e = mirvar(0);
        while (!found){
        	strong_bigdig(&rng, le, 2, result.e);
                found = isprime(result.e);
        }

	strong_bigdig(&rng, ls, 2, result.s);
gettimeofday(&t1, 0);
psignofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp7, temp8, temp9, temp11;
	temp7 = mirvar(data);
	temp8 = mirvar(data1);
	temp9 = mirvar(0);
	temp11 = mirvar(0);
	powmod(pks.a1, temp7, pks.n, temp7);
	powmod(pks.a2, temp8, pks.n, temp8);
gettimeofday(&t1, 0);
psignonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(pks.b, result.s, pks.n, temp9);
	mad(temp9, pks.c, pks.c, pks.n, pks.n, temp9);
gettimeofday(&t1, 0);
psignofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(temp7, temp8, temp8, pks.n, pks.n, temp7);
	mad(temp7, temp9, temp9, pks.n, pks.n, temp7);
gettimeofday(&t1, 0);
psignonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	multi_inverse (1, &result.e, sks.fi, &temp11);
gettimeofday(&t1, 0);
psignofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(temp7, temp11, pks.n, result.v);
gettimeofday(&t1, 0);
psignonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// producing commitment
gettimeofday(&t0, 0);
	strong_bigdig(&rng, ln, 2, result.rx);
	rx = mirvar(0);
	copy(result.rx, rx);
gettimeofday(&t1, 0);
csignofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp10;
	temp10 = mirvar(0);
	powmod(pkc.g, result.x, pkc.n, temp10);
gettimeofday(&t1, 0);
csignonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(pkc.h, result.rx, pkc.n, result.commit);
gettimeofday(&t1, 0);
csignofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(result.commit, temp10, temp10, pkc.n, pkc.n, result.commit);
gettimeofday(&t1, 0);
csignonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
			
	// PR broadcasts the result

        return result;
}

// COT step 1
struct OutputsCOT1 COT1(int data, int data1, struct Com com, big v, big e, big s, big commit)
{
	// this part is computed by PR
gettimeofday(&t0, 0);
        struct OutputsCOT1 result;
	result.Msg = 1;
	result.idsPS = Ps;
	result.idsPR = Pr;
	result.idscid = CidR;
	result.idssid = Sid;
	result.comu = mirvar(0);
	result.come = mirvar(0);
	result.comv = mirvar(0);
		
	big temp1;
	temp1 = mirvar(0);
	sftbit(pk.n, -2, temp1);
	x = mirvar(0);
	strong_bigrand(&rng, temp1, x);
	powmod(pk.g, x, n2, result.comv);
	powmod(pk.g, r, n2, result.comu);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp2;
	temp2 = mirvar(data);
	powmod(alpha, temp2, n2, temp2);
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp3;
	temp3 = mirvar(0);
        powmod(result.comv, r, n2, temp3);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
        mad(temp3, temp2, temp2, n2, n2, result.come);
gettimeofday(&t1, 0);	
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// PR sends result to PS
	
	// this part is computed by PR
gettimeofday(&t0, 0);
	big temp4;
	temp4 = mirvar(0);
	copy(result.comv, temp4);
	xgcd(temp4, n2, temp4, temp4, temp4);
	mad(pk.g1, temp4, temp4, n2, n2, temp4);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);		
	big temp5;
	temp5 = mirvar(0);
	copy(result.come, temp5);
	xgcd(temp5, n2, temp5, temp5, temp5);
	mad(com.e, temp5, temp5, n2, n2, temp5);
	
	big temp6;
	temp6 = mirvar(0);
	copy(com.e, temp6);
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	int a = ZKDLEQ(pk.g, result.comu, temp4, temp5);
	int b = ZKCom(pk, com, result.idsPR, result.idscid, data);
	int c = ZKBit(temp6, data);
	int ee;
	ee = ZKBitC(commit, data);
	big datap, data1p;
	datap = mirvar(data);
	data1p = mirvar(data1);
	int d = ZKSign(datap, data1p, v, e, s, rx, commit);
gettimeofday(&t0, 0);		
	// compute intersection of a and b 
	if (a == 0 || b == 0 || c == 0 || d == 0 || ee == 0)
		printf("They should stop their computations.\n");	
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;		
        return result;
}

// COT step 2
struct OutputsCOT2 COT2(struct OutputsCOT1 data, big m0, big m1)
{
	// This part is computing by PS
        struct OutputsCOT2 result;
	result.Msg = 2;
	result.idsPS = data.idsPS;
	result.idsPR = data.idsPR;
	result.idscid = data.idscid;
	result.idssid = data.idssid;
	result.u0 = mirvar(0);
	result.e0 = mirvar(0);
	result.u1 = mirvar(0);
	result.e1 = mirvar(0);
gettimeofday(&t0, 0);		
	big s0, s1, r0, r1, temp1, temp2;
	s0 = mirvar(0);
	s1 = mirvar(0);
	r0 = mirvar(0);
	r1 = mirvar(0);
	temp1 = mirvar(0);
	temp2 = mirvar(0);
	sftbit (pk.n, -1, temp1);
	sftbit (pk.n, 1, temp2);
		
	BOOL found = FALSE;
		
	while (!found){
		strong_bigrand(&rng, temp1, r0);
		if (remain (r0, 2) == 0)
			found = TRUE;
	}
		
	found = FALSE;
	while (!found){
		strong_bigrand(&rng, temp1, r1);
		if (remain (r1, 2) == 0)
			found = TRUE;
	}
		
	found = FALSE;
	while (!found){
		strong_bigrand(&rng, temp2, s0);
		if (remain (s0, 2) == 0)
			found = TRUE;
	}
		
	found = FALSE;
	while (!found){
		strong_bigrand(&rng, temp2, s1);
		if (remain (s1, 2) == 0)
			found = TRUE;
	}
gettimeofday(&t1, 0);
cotofs += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp00;
	temp00 = mirvar(0);
	powmod(data.come, s0, n2, temp00);
	
	big temp01;
	temp01 = mirvar(0);
	powmod(data.come, s1, n2, temp01);

	big temp10;
	temp10 = mirvar(0);
	powmod(alpha, m0, n2, temp10);
gettimeofday(&t1, 0);
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);		
	big temp11, temp;
	temp11 = mirvar(0);
	temp = mirvar(0);
	powmod(alpha, s1, n2, temp);
	xgcd(temp, n2, temp, temp, temp);
gettimeofday(&t1, 0);
cotofs += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(alpha, m1, n2, temp11);
	mad(temp11, temp, temp, n2, n2, temp11);
		
	big temp20;
	temp20 = mirvar(0);
	powmod(data.comv, r0, n2, temp20);
		
	big temp21;
	temp21 = mirvar(0);
	powmod(data.comv, r1, n2, temp21);
		
	mad(temp00, temp10, temp10, n2, n2, result.e0);
	mad(temp20, result.e0, result.e0, n2, n2, result.e0);
		
	mad(temp01, temp11, temp11, n2, n2, result.e1);
	mad(temp21, result.e1, result.e1, n2, n2, result.e1);
		
	big temp30;
	temp30 = mirvar(0);
	powmod(data.comu, s0, n2, temp30);
	
	big temp31;
	temp31 = mirvar(0);
	powmod(data.comu, s1, n2, temp31);
gettimeofday(&t1, 0);
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);		
	big temp40;
	temp40 = mirvar(0);
	powmod(pk.g, r0, n2, temp40);
		
	big temp41;
	temp41 = mirvar(0);
	powmod(pk.g, r1, n2, temp41);
gettimeofday(&t1, 0);
cotofs += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);		
	mad(temp30, temp40, temp40, n2, n2, result.u0);
		
	mad(temp31, temp41, temp41, n2, n2, result.u1);
gettimeofday(&t1, 0);	
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;	
	// if PS passed the proofs, it will send the result
		
        return result;
}

// COT step 3
big COT3(struct OutputsCOT2 cipher, int data)
{
gettimeofday(&t0, 0);
	big u[2];
	u[0] = mirvar(0);
	copy(cipher.u0, u[0]);
	u[1] = mirvar(0);
	copy(cipher.u1, u[1]);
	
	big e[2];
	e[0] = mirvar(0);
	copy(cipher.e0, e[0]);
	e[1] = mirvar(0);
	copy(cipher.e1, e[1]);
	
	big temp1, temp2;
	temp1 = mirvar(0);
	temp2 = mirvar(0);
	powmod(u[data], x, n2, temp1);
	xgcd(temp1, n2, temp1, temp1, temp1);
	mad(e[data], temp1, temp1, n2, n2, temp2);
	
	big temp3, temp4;
	temp3 = mirvar(2);
	temp4 = mirvar(0);
	powmod(temp2, temp3, n2, temp2);
	decr(temp2, 1, temp2);
	if (divisible (temp2, pk.n) == FALSE)
		printf("Reject.\n");
	copy(pk.n, temp4);
	divide(temp2, temp4, temp2);
	
	big bigm, one;
	bigm = mirvar(0);
	one = mirvar(1);
	sftbit (temp2, -1, bigm);
	powmod(bigm, one, pk.n, bigm);
	big res;
	res = mirvar(0);
	copy(pk.n, res);
	sftbit(res, -1, res);
	if (mr_compare(res, bigm) == -1)
		subtract(bigm, pk.n, bigm);	
gettimeofday(&t1, 0);	
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	return bigm;
}

// Collision resistance hash function
big Hash(big k, big u, big e, int idsp, int idscid)
{
	big result;
	result = mirvar(0);
	big p;
	p = mirvar(idsp);
	big cid;
	cid = mirvar(idscid);
	
	char buffer[K/2 + 2*length + 2];
    	bzero(buffer, K/2 + 2*length + 2);
    	int len;

    	len = big_to_bytes(K/2, k, buffer, TRUE);
	len = big_to_bytes(length, u, buffer + sizeof(char) * K/2, TRUE);
	len = big_to_bytes(length, e, buffer + sizeof(char) * (K/2 + length), TRUE);
	len = big_to_bytes(1, p, buffer + sizeof(char) * (K/2 + 2*length), TRUE);
	len = big_to_bytes(1, cid, buffer + sizeof(char) * (K/2 + 2*length + 1), TRUE);
	
	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
   	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < (K/2 + 2*length + 2); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh , hash);
	
	char temp1[32];
	for (i = 0; i < 32; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(32, temp1, result);

	return result;
}

// ZKDLEQ
int ZKDLEQ(big g, big x1, big gp, big x2)
{ 
	// this part is computed by PR
gettimeofday(&t0, 0);
	big zero;
	zero = mirvar(0);
	big ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pk.n, temp1, ub);
	
	big alphap;
	alphap = mirvar(0);
	strong_bigrand(&rng, ub, alphap);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);	
	big x1p;
	x1p = mirvar(0);
	powmod (g, alphap, n2, x1p);
	
	big x2p;
	x2p = mirvar(0);
	powmod (gp, alphap, n2, x2p);
	
	big x1p2;
	x1p2 = mirvar(2);
	powmod(x1p, x1p2, n2, x1p2); 

	big x2p2;
	x2p2 = mirvar(2);
	powmod(x2p, x2p2, n2, x2p2);
	
	big c;
	c = mirvar(0);
	c = HashZK(g, x1, gp, x2, x1p2, x2p2);
	
	big alphat;
	alphat = mirvar(0);
	big temp4;
	temp4 = mirvar(0);
	multiply(r, c, temp4);
	subtract(alphap, temp4, alphat);
gettimeofday(&t1, 0);	
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// PR sends x1p2, x2p2, c, and alphat to PS
	
	// this part is computed by PS
gettimeofday(&t0, 0);
	big c2;
	c2 = mirvar(0);
	add(c, c, c2);
	big alphat2;
	alphat2 = mirvar(2);
	mad(alphat2, alphat, alphat, n2, n2, alphat2);
	
	big temp5, temp6;
	temp5 = mirvar(0);
	temp6 = mirvar(0);
	powmod(x1, c2, n2, temp5);
	powmod(g, alphat2, n2, temp6);
	mad(temp5, temp6, temp6, n2, n2, temp5);
	
	big temp7, temp8;
	temp7 = mirvar(0);
	temp8 = mirvar(0);
	powmod(x2, c2, n2, temp7);
	powmod(gp, alphat2, n2, temp8);
	mad(temp7, temp8, temp8, n2, n2, temp7);

	int re;	
	if (mr_compare(temp5, x1p2) == 0 && mr_compare(temp7, x2p2) == 0)
		re = 1;
	else 
		re = 0;
gettimeofday(&t1, 0);
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	
	return re;
}

// ZKCom
int ZKCom(struct PK Pk, struct Com com, int idsPR, int idscid, int data)
{
	// this part is computed by PR
gettimeofday(&t0, 0);
	big ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pk.n, temp1, ub);

	big rp;
	rp = mirvar(0);
	strong_bigrand(&rng, ub, rp);
	
	big mp;
	mp = mirvar(0);
	strong_bigrand(&rng, ub, mp);

	big two;
	two = mirvar(2);
	
	struct Com com1;
	com1.u = mirvar(0);
	powmod(Pk.g, rp, n2, com1.u);
	powmod(com1.u, two, n2, com1.u);
	
	com1.e = mirvar(0);
	big temp4;
	temp4 = mirvar(0);
	powmod(alpha, mp, n2, temp4);
	powmod(Pk.g1, rp, n2, com1.e);
	mad(temp4, com1.e, com1.e, n2, n2, com1.e);
	powmod(com1.e, two, n2, com1.e);
gettimeofday(&t1, 0);
cotofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	com1.v = mirvar(0);	
	big temp5;
	temp5 = mirvar(0);
	temp5 = Hash(Pk.k, com.u, com.e, idsPR, idscid);
		
	big temp6;
	temp6 = mirvar(0);
	powmod(Pk.g3, temp5, n2, temp6);
    	mad(temp6, Pk.g2, Pk.g2, n2, n2, temp6);
	powmod(temp6, rp, n2, com1.v);
	powmod(com1.v, two, n2, com1.v);

	big c;
	c = mirvar(0);
	c = HashZK(com.u, com.e, com.v, com1.u, com1.e, com1.v);

	big rt;
	rt = mirvar(0);
	big temp7;
	temp7 = mirvar(0);
	multiply(r, c, temp7);
	subtract(rp, temp7, rt);	
		
	big mt;
	mt = mirvar(0);
	big temp8;
	temp8 = mirvar(data);
	multiply(temp8, c, temp8);
	subtract(mp, temp8, mt);
gettimeofday(&t1, 0);
cotonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;	
	// PR sends com1, c, rt, and mt to PS
	
	// this part is computed by PS	
gettimeofday(&t0, 0);
	big c2;
	c2 = mirvar(0);
	add(c, c, c2);	
	big rt2;
	rt2 = mirvar(0);
	add(rt, rt, rt2);
	big mt2;
	mt2 = mirvar(0);
	add(mt, mt, mt2);
	
	big temp9, temp10;
	temp9 = mirvar(0);
	temp10 = mirvar(0);
	powmod(com.u, c2, n2, temp9);
	powmod(Pk.g, rt2, n2, temp10);
	mad(temp9, temp10, temp10, n2, n2, temp9);
	
	big temp11, temp12, temp13;
	temp11 = mirvar(0);
	temp12 = mirvar(0);
	temp13 = mirvar(0);
	powmod(com.e, c2, n2, temp11);
	powmod(alpha, mt2, n2, temp12);
	powmod(Pk.g1, rt2, n2, temp13);
	mad(temp11, temp12, temp12, n2, n2, temp11);
	mad(temp11, temp13, temp13, n2, n2, temp11);
	
	big temp14, temp15;
	temp14 = mirvar(0);
	temp15 = mirvar(0);
	temp14 = Hash(Pk.k, com.u, com.e, idsPR, idscid);
	powmod(Pk.g3, temp14, n2, temp14);
	mad(Pk.g2, temp14, temp14, n2, n2, temp14);
	powmod(temp14, rt2, n2, temp14);
	powmod(com.v, c2, n2, temp15);
	mad(temp15, temp14, temp14, n2, n2, temp14);

	int re;	
	if (mr_compare(temp9, com1.u) == 0 && mr_compare(temp11, com1.e) == 0 && mr_compare(temp14, com1.v) == 0)
		re = 1;
	else 
		re = 0;
gettimeofday(&t1, 0);
cotons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return re;
}

// Collision resistance hash function for challenge in ZKCom & ZKDLEQ 
big HashZK(big a, big b, big c, big d, big e, big f)
{
	big result;
	result = mirvar(0);
	
	char buffer[6*length];
    	bzero(buffer, 6*length);
    	int len;

	len = big_to_bytes(length, a, buffer, TRUE);
	len = big_to_bytes(length, b, buffer + sizeof(char) * length, TRUE);
	len = big_to_bytes(length, c, buffer + sizeof(char) * 2*length, TRUE);
	len = big_to_bytes(length, d, buffer + sizeof(char) * 3*length, TRUE);
	len = big_to_bytes(length, e, buffer + sizeof(char) * 4*length, TRUE);
	len = big_to_bytes(length, f, buffer + sizeof(char) * 5*length, TRUE);

	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < (6*length); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh , hash);
	
	char temp1[16];
	for (i = 0; i < 16; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(16, temp1, result);
	
	return result;
}

// ZKBit
int ZKBit(big c, int data)
{
	// this part is computed by PR
gettimeofday(&t0, 0);
	big  ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pk.n, temp1, ub);
	
	big v1;
	v1 = mirvar(0);
	strong_bigrand(&rng, ub, v1);
	
	big v2;
	v2 = mirvar(0);
	strong_bigrand(&rng, ub, v2);

	big w;
	w = mirvar(0);
	expb2(kk, ub);
	strong_bigrand(&rng, ub, w);

	big temp5;
	temp5 = mirvar(0);
	copy(alpha, temp5);
	xgcd(temp5, n2, temp5, temp5, temp5);
gettimeofday(&t1, 0);
bit2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(temp5, c, c, n2, n2, temp5);

	big t1t, t2, c1, c2, r1, r2, cp;
	big sub, one;
	sub = mirvar(0);
	one = mirvar(1);

	if (data == 1)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(c, w, n2, temp4);
		powmod(pk.g1, v1, n2, t1t);
		mad(temp4, t1t, t1t, n2, n2, t1t);
		powmod(pk.g1, v2, n2, t2);

		big ch = HashBit(pk.g1, c, temp5, t1t, t2);
		
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		copy(w, c1);
		subtract(ub, c1, sub);
		add(sub, ch, sub);
		powmod(sub, one, ub, c2);
		copy(v1, r1);
		multiply(c2, r, temp6);
		subtract(v2, temp6, r2);

		// PR sends t1, t2, c, c1, c2, r1, r2 tp PS
	}
	if (data == 0)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(pk.g1, v1, n2, t1t);
		powmod(pk.g1, v2, n2, t2);
		powmod(temp5, w, n2, temp4);
		mad(temp4, t2, t2, n2, n2, t2);

		big ch = HashBit(pk.g1, c, temp5, t1t, t2);
		big  temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		copy(w, c2);
		subtract(ub, c2, sub);
		add(sub, ch, sub);
		powmod(sub, one, ub, c1);
		copy(v2, r2);
		multiply(c1, r, temp6);
		subtract(v1, temp6, r1);

		// PR sends t1, t2, c, c1, c2, r1, r2 to PS
	}

gettimeofday(&t1, 0);
bit2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// this part is computed by PS
gettimeofday(&t0, 0);
	big t1p, t2p, temp7;
	t1p = mirvar(0);
	t2p = mirvar(0);
	temp7 = mirvar(0);
	powmod(c, c1, n2, temp7);
	powmod(pk.g1, r1, n2, t1p);
	mad(temp7, t1p, t1p, n2, n2, t1p);
	powmod(temp5, c2, n2, temp7);
	powmod(pk.g1, r2, n2, t2p);
	mad(temp7, t2p, t2p, n2, n2, t2p);
	
	big ch1;
	ch1 = mirvar(0);
	ch1 = HashBit(pk.g1, c, temp5, t1p, t2p);
	cp = mirvar(0);
	add(c1, c2, cp);
	powmod(cp, one, ub, cp);

	int aa = mr_compare(t1t, t1p);
	int bb = mr_compare(t2, t2p);
	int cc = mr_compare(cp, ch1);;

	int re;
	if(aa == 0 && bb == 0 && cc ==0)
		re = 1;
	else
		re = 0;

gettimeofday(&t1, 0);
bit2ons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return re;
}

// ZKBit of new commitment
int ZKBitC(big c, int data)
{
	// this part is computed by PR
gettimeofday(&t0, 0);
	big  ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pkc.n, temp1, ub);
	
	big v1;
	v1 = mirvar(0);
	strong_bigrand(&rng, ub, v1);
	
	big v2;
	v2 = mirvar(0);
	strong_bigrand(&rng, ub, v2);

	big w;
	w = mirvar(0);
	expb2(kk, ub);
	strong_bigrand(&rng, ub, w);

	big temp5;
	temp5 = mirvar(0);
	copy(pkc.g, temp5);
	xgcd(temp5, pkc.n, temp5, temp5, temp5);
gettimeofday(&t1, 0);
bit1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(temp5, c, c, pkc.n, pkc.n, temp5);

	big t1t, t2, c1, c2, r1, r2, cp;
	big sub, one;
	sub = mirvar(0);
	one = mirvar(1);

	if (data == 1)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(c, w, pkc.n, temp4);
		powmod(pkc.h, v1, pkc.n, t1t);
		mad(temp4, t1t, t1t, pkc.n, pkc.n, t1t);
		powmod(pkc.h, v2, pkc.n, t2);

		big ch = HashBit(pkc.h, c, temp5, t1t, t2);
		
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c1 = w;
		subtract(ub, c1, sub);
		add(sub, ch, sub);
		powmod(sub, one, ub, c2);
		r1 = v1;
		multiply(c2, rx, temp6);
		subtract(v2, temp6, r2);

		// PR sends t1, t2, c, c1, c2, r1, r2 tp PS
	}
	if (data == 0)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(pkc.h, v1, pkc.n, t1t);
		powmod(pkc.h, v2, pkc.n, t2);
		powmod(temp5, w, pkc.n, temp4);
		mad(temp4, t2, t2, pkc.n, pkc.n, t2);

		big ch = HashBit(pkc.h, c, temp5, t1t, t2);

		big  temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c2 = w;
		subtract(ub, c2, sub);
		add(sub, ch, sub);
		powmod(sub, one, ub, c1);
		r2 = v2;
		multiply(c1, rx, temp6);
		subtract(v1, temp6, r1);

		// PR sends t1, t2, c, c1, c2, r1, r2 to PS
	}
gettimeofday(&t1, 0);
bit1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// this part is computed by PS
gettimeofday(&t0, 0);
	big t1p, t2p, temp7;
	t1p = mirvar(0);
	t2p = mirvar(0);
	temp7 = mirvar(0);
	powmod(c, c1, pkc.n, temp7);
	powmod(pkc.h, r1, pkc.n, t1p);
	mad(temp7, t1p, t1p, pkc.n, pkc.n, t1p);
	powmod(temp5, c2, pkc.n, temp7);
	powmod(pkc.h, r2, pkc.n, t2p);
	mad(temp7, t2p, t2p, pkc.n, pkc.n, t2p);
	
	big ch1;
	ch1 = mirvar(0);
	ch1 = HashBit(pkc.h, c, temp5, t1p, t2p);

	int aa = mr_compare(t1t, t1p);
	int bb = mr_compare(t2, t2p);
	cp = mirvar(0);
	add(c1, c2, cp);
	powmod(cp, one, ub, cp);
	int cc = mr_compare(cp, ch1);

	int re;
	if(aa == 0 && bb == 0 && cc ==0)
		re = 1;
	else
		re = 0;
gettimeofday(&t1, 0);
bit1ons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return re;
}

// Collision resistance hash function for challenge in ZKBit
big HashBit(big a, big b, big c, big d, big e)
{
	big result;
	result = mirvar(0);
	
	char buffer[5*length];
    	bzero(buffer, 5*length);
    	int len;

	len = big_to_bytes(length, a, buffer, TRUE);
	len = big_to_bytes(length, b, buffer + sizeof(char) * length, TRUE);
	len = big_to_bytes(length, c, buffer + sizeof(char) * 2*length, TRUE);
	len = big_to_bytes(length, d, buffer + sizeof(char) * 3*length, TRUE);
	len = big_to_bytes(length, e, buffer + sizeof(char) * 4*length, TRUE);

	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < (5*length); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh , hash);
	
	char temp1[16];
	for (i = 0; i < 16; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(16, temp1, result);

	return result;
}

int ZKSign(big data, big data1, big v, big e, big s, big rx, big commit)
{
	// this part is computing by PR	
gettimeofday(&t0, 0);
	big w, rw, cv, cw;
	w = mirvar(0);
	rw = mirvar(0);
	cv = mirvar(0);
	cw = mirvar(0);
	strong_bigdig(&rng, ln, 2, w);
	strong_bigdig(&rng, ln, 2, rw);
	big temp1, temp11, temp111;
	temp1 = mirvar(0);
	temp11 = mirvar(0);
	temp111 = mirvar(0);
	powmod(pkc.g, w, pkc.n, temp1);
copy(temp1, temp111);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(v, temp1, temp1, pkc.n, pkc.n, cv);
gettimeofday(&t1, 0);
signonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	//powmod(pkc.g, w, pkc.n, temp111);
	powmod(pkc.h, rw, pkc.n, temp11);
	mad(temp111, temp11, temp11, pkc.n, pkc.n, cw);

	big ep, xp, rxp, sp, ewp, wp, rwp, erwp;
	ep = mirvar(0);
	xp = mirvar(0);
	rxp = mirvar(0);
	sp = mirvar(0);
	ewp = mirvar(0);
	wp = mirvar(0);
	rwp = mirvar(0);
	erwp = mirvar(0);
	strong_bigdig(&rng, le+kk+kp-1, 2, ep);
	strong_bigdig(&rng, lm+kk+kp-1, 2, xp);
	strong_bigdig(&rng, ln+kk+kp-1, 2, rxp);
	strong_bigdig(&rng, ls+kk+kp-1, 2, sp);
	strong_bigdig(&rng, le+ln+kk+kp-1, 2, ewp);
	strong_bigdig(&rng, ln+kk+kp-1, 2, wp);
	strong_bigdig(&rng, ln+kk+kp-1, 2, rwp);
	strong_bigdig(&rng, le+ln+kk+kp-1, 2, erwp);
	big c1, c2, c3, c4;
	c1 = mirvar(0);
	c2 = mirvar(0);
	c3 = mirvar(0);
	c4 = mirvar(0);
	big temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp101;
	temp2 = mirvar(0);
	temp3 = mirvar(0);
	temp4 = mirvar(0);
	temp5 = mirvar(0);
	temp6 = mirvar(0);
	temp7 = mirvar(0);
	temp8 = mirvar(0);
	temp9 = mirvar(0);
	temp10 = mirvar(0);
	temp101 = mirvar(0);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pks.a1, temp2);
	xgcd(temp2, pks.n, temp2, temp2, temp2);
gettimeofday(&t0, 0);
	powmod(temp2, xp, pks.n, temp2);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pks.b, temp3);
	xgcd(temp3, pks.n, temp3, temp3, temp3);
gettimeofday(&t0, 0);
	powmod(temp3, sp, pks.n, temp3);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.g, temp4);
	xgcd(temp4, pkc.n, temp4, temp4, temp4);
gettimeofday(&t0, 0);
	powmod(temp4, ewp, pkc.n, temp4);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.h, temp5);
	xgcd(temp5, pkc.n, temp5, temp5, temp5);
gettimeofday(&t0, 0);
	powmod(temp5, erwp, pkc.n, temp5);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(cv, ep, pkc.n, temp6);
	mad(temp6, temp2, temp2, pkc.n, pkc.n, c1);
gettimeofday(&t1, 0);
signonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	mad(c1, temp3, temp3, pkc.n, pkc.n, c1);
	mad(c1, temp4, temp4, pkc.n, pkc.n, c1);
	powmod(pkc.g, wp, pkc.n, temp7);
	powmod(pkc.h, rwp, pkc.n, temp8);
	mad(temp7, temp8, temp8, pkc.n, pkc.n, c2);
	powmod(cw, ep, pkc.n, temp9);
	mad(temp9, temp4, temp4, pkc.n, pkc.n, c3);
	mad(c3, temp5, temp5, pkc.n, pkc.n, c3);
	powmod(pkc.h, rxp, pkc.n, temp10);
	powmod(pkc.g, xp, pkc.n, temp101);
	mad(temp101, temp10, temp10, pkc.n, pkc.n, c4);
gettimeofday(&t1, 0);
signofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big c;
	c = mirvar(0);
	c = HashZKSign(pkc.n, pks.a1, pks.a2, pks.b, pks.c, pkc.g, pkc.h, commit, cv, cw, c1, c2, c3, c4);

	big et, xt, rxt, st, ewt, wt, rwt, erwt;
	et = mirvar(0);
	xt = mirvar(0);
	rxt = mirvar(0);
	st = mirvar(0);
	ewt = mirvar(0);
	wt = mirvar(0);
	rwt = mirvar(0);
	erwt = mirvar(0); 
	
	multiply(c, e, et);
copy(et, ewt);
copy(et, erwt);	
subtract(ep, et, et);
	multiply(c, data, xt);
	subtract(xp, xt, xt);
	multiply(c, rx, rxt);
	subtract(rxp, rxt, rxt);
	multiply(c, s, st);
	subtract(sp, st, st);
	//multiply(c, e, ewt);
	multiply(ewt, w, ewt);
	subtract(ewp, ewt, ewt);
	multiply(c, w, wt);
	subtract(wp, wt, wt);
	multiply(c, rw, rwt);
	subtract(rwp, rwt, rwt);
	//multiply(c, e, erwt);
	multiply(erwt, rw, erwt);
	subtract(erwp, erwt, erwt);
gettimeofday(&t1, 0);
signonc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// PR sends c1, c2, c3, c4, m2, et, xt, rxt, st, ewt, wt, rwt, and erwt to PS

	// this part is computing by PS
gettimeofday(&t0, 0);
	big A2, Ve, temp12, temp121; 
	A2 = mirvar(0);
	Ve = mirvar(0);
	temp12 = mirvar(0);
	temp121 = mirvar(0);
	powmod(pks.a1, data, pks.n, temp12);
	powmod(pks.a2, data1, pks.n, A2);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	powmod(pks.b, s, pks.n, temp121);
	mad(temp12, A2, A2, pks.n, pks.n, Ve);
gettimeofday(&t0, 0);
	mad(Ve, temp121, temp121, pks.n, pks.n, Ve);
	mad(Ve, pks.c, pks.c, pks.n, pks.n, Ve);

	big c1p, c2p, c3p, c4p;
	c1p = mirvar(0);
	c2p = mirvar(0);
	c3p = mirvar(0);
	c4p = mirvar(0);
	big temp13, temp14, temp15, temp16, temp17, temp18, temp19, temp20, temp21, temp22, temp23, temp24, temp25, temp26;
	temp13 = mirvar(0);
	temp14 = mirvar(0);
	temp15 = mirvar(0);
	temp16 = mirvar(0);
	temp17 = mirvar(0);
	temp18 = mirvar(0);
	temp19 = mirvar(0);
	temp20 = mirvar(0);
	temp21 = mirvar(0);
	temp22 = mirvar(0);
	temp23 = mirvar(0);
	temp24 = mirvar(0);
	temp25 = mirvar(0);
	temp26 = mirvar(0);
	mad(pks.c, A2, A2, pks.n, pks.n, temp13);
	powmod(temp13, c, pks.n, temp13);
	powmod(cv, et, pks.n, temp14);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pks.a1, temp15);
	xgcd(temp15, pks.n, temp15, temp15, temp15);
gettimeofday(&t0, 0);
	powmod(temp15, xt, pks.n, temp15);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pks.b, temp16);
	xgcd(temp16, pks.n, temp16, temp16, temp16);
gettimeofday(&t0, 0);
	powmod(temp16, st, pks.n, temp16);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.g, temp17);
	xgcd(temp17, pks.n, temp17, temp17, temp17);
gettimeofday(&t0, 0);
	powmod(temp17, ewt, pks.n, temp17);
	mad(temp13, temp14, temp14, pks.n, pks.n, c1p);
	mad(temp15, c1p, c1p, pks.n, pks.n, c1p);
	mad(temp16, c1p, c1p, pks.n, pks.n, c1p);
	mad(temp17, c1p, c1p, pks.n, pks.n, c1p);
	int a = mr_compare(c1p, c1);
	powmod(cw, c, pks.n, temp18);
	powmod(pkc.g, wt, pks.n, temp19);
	powmod(pkc.h, rwt, pks.n, temp20);
	mad(temp18, temp19, temp19, pks.n, pks.n, c2p);
	mad(temp20, c2p, c2p, pks.n, pks.n, c2p);
	int b = mr_compare(c2p, c2);
	powmod(cw, et, pks.n, temp21);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.h, temp23);
	xgcd(temp23, pks.n, temp23, temp23, temp23);
gettimeofday(&t0, 0);
	powmod(temp23, erwt, pks.n, temp23);
	mad(temp21, temp17, temp17, pks.n, pks.n, c3p);
	mad(temp23, c3p, c3p, pks.n, pks.n, c3p);
	int cc = mr_compare(c3p, c3);
	powmod(commit, c, pks.n, temp24);
	powmod(pkc.g, xt, pks.n, temp25);
	powmod(pkc.h, rxt, pks.n, temp26);
	mad(temp24, temp25, temp25, pks.n, pks.n, c4p);
	mad(temp26, c4p, c4p, pks.n, pks.n, c4p);
	int d = mr_compare(c4p, c4);
gettimeofday(&t1, 0);
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	big temp27, temp28;
	temp27 = mirvar(0);
	temp28 = mirvar(0);
	expb2 (le+kk+kp-2, temp27);
	expb2 (le+kk+kp-1, temp28);
gettimeofday(&t1, 0);
signofs += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	int ee = 1;
	if (mr_compare(temp27, et) == -1 && mr_compare(temp28, et) == 1)
		ee = 0;

	big temp29;
	temp29 = mirvar(0);
	temp29 = HashZKSign(pkc.n, pks.a1, pks.a2, pks.b, pks.c, pkc.g, pkc.h, commit, cv, cw, c1p, c2p, c3p, c4p);
	int f = mr_compare(c, temp29);

	int re;
	if (a == 0 && b == 0 && cc == 0 && d == 0 && ee == 0 && f == 0)
		re = 1;
	else
		re = 0;
gettimeofday(&t1, 0);	
signons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return re;
}

big HashZKSign(big a1, big a2, big a3, big a4, big a5, big a6, big a7, big a8, big a9, big a10, big a11, big a12, big a13, big a14)
{
	big result;
	result = mirvar(0);
	char buffer[14*ln];
    	bzero(buffer, 14*ln);
    	int len;

	len = big_to_bytes(ln, a1, buffer, TRUE);
	len = big_to_bytes(ln, a2, buffer + sizeof(char) * ln, TRUE);
	len = big_to_bytes(ln, a3, buffer + sizeof(char) * 2*ln, TRUE);
	len = big_to_bytes(ln, a4, buffer + sizeof(char) * 3*ln, TRUE);
	len = big_to_bytes(ln, a5, buffer + sizeof(char) * 4*ln, TRUE);
	len = big_to_bytes(ln, a6, buffer + sizeof(char) * 5*ln, TRUE);
	len = big_to_bytes(ln, a7, buffer + sizeof(char) * 6*ln, TRUE);
	len = big_to_bytes(ln, a8, buffer + sizeof(char) * 7*ln, TRUE);
	len = big_to_bytes(ln, a9, buffer + sizeof(char) * 8*ln, TRUE);
	len = big_to_bytes(ln, a10, buffer + sizeof(char) * 9*ln, TRUE);
	len = big_to_bytes(ln, a11, buffer + sizeof(char) * 10*ln, TRUE);
	len = big_to_bytes(ln, a12, buffer + sizeof(char) * 11*ln, TRUE);
	len = big_to_bytes(ln, a13, buffer + sizeof(char) * 12*ln, TRUE);
	len = big_to_bytes(ln, a14, buffer + sizeof(char) * 13*ln, TRUE);

	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < (14*ln); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh , hash);

	char temp1[16];
	for (i = 0; i < 16; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(16, temp1, result);

	return result;
}

void printnum(big me)
{
        char number[length];
        int len, i;
        len = big_to_bytes(length, me, number, FALSE);
        printf("%d %d ", numdig(me), len);
        for (i = 0; i < len; i++) 
        {
                printf("%02x", number[i] & 0xff);
        }  
        printf("\n"); 

        return;  
}

// key generation
void KeyGeneratorPaternity()
{    
        BOOL found = FALSE;
	BOOL found1 = FALSE;
        long int count = 0;
	
	big temp, two;
	temp = mirvar(0);
	two = mirvar(2);

        //generating q
	pkp.q = mirvar(0);
	pkp.p = mirvar(0);
	while(!found1){
		found = FALSE;
        	while (!found){
        		strong_bigdig(&rng, 192, 2, pkp.q);
                	found = isprime(pkp.q);
                	count++;
        	}
		strong_bigdig(&rng, K-192, 2, temp);
		multiply(temp, two, temp);
		multiply(temp, pkp.q, pkp.p);
		//add(temp, temp, temp);
		incr(pkp.p, 1, pkp.p);
		found1 = isprime(pkp.p);
	}

	printf("number of trials for p: %ld\n", count);
	
	// computing g
	found = FALSE;
	big gp, res, one, pow;
	gp = mirvar(0);
	res = mirvar(0);
	one = mirvar(1);
	pkp.g = mirvar(0);
	pow = mirvar(0);
	decr(pkp.p, 1, pow);
	while(!found){
		strong_bigrand(&rng, pkp.p, gp);
		powmod(gp, temp, pkp.p, res);
		if (mr_compare(res, one) != 0)
			found = TRUE;
	}
	copy(gp, pkp.g);
	
        return;
}

// store key on the file
void store_keyP()
{
        FILE *fp;
   
        fp = fopen("PublicPaternity.txt", "w");
        mip->IOBASE = 16;
	cotnum(pkp.q, fp);
	cotnum(pkp.p, fp);
        cotnum(pkp.g, fp);

        fclose(fp);

        return;
}

// read key from the file
void read_keyP() 
{
        FILE *fp;
        fp = fopen("PublicPaternity.txt", "r");
        mip->IOBASE = 16;
        pkp.q = mirvar(0);
        pkp.p = mirvar(0);
	pkp.g = mirvar(0);
		
        cinnum(pkp.q, fp);
        cinnum(pkp.p, fp);
	cinnum(pkp.g, fp);
	
        return;
}

// OT
big OT(int data, big m0, big m1)
{
	big result;
	result = mirvar(0);

	// this part is computing by sender
	gettimeofday(&t0, 0);
	big c, rc;
	c = mirvar(0);
	rc = mirvar(0);
	strong_bigrand(&rng, pkp.q, c);
	strong_bigrand(&rng, pkp.q, rc);
	
	big gr, cr;
	gr = mirvar(0);
	cr = mirvar(0);
	powmod(pkp.g, rc, pkp.p, gr);
	powmod(c, rc, pkp.p, cr);
	gettimeofday(&t1, 0);
	ofsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// sender publishes c

	// this part is compuring by chooser
	gettimeofday(&t0, 0);
	big k;
	k = mirvar(0);	
	big temp;
	temp = mirvar(0);
	copy(pkp.q, temp);
	incr(temp, 1, temp);
	strong_bigrand(&rng, temp, k);
	copy(k, khat);

	big pkdata[2];
	pkdata[0] = mirvar(0);
	pkdata[1] = mirvar(0);
	
	big temp1;
	temp1 = mirvar(0);
	powmod(pkp.g, k, pkp.p, temp1);
	copy(temp1, pkdata[data]);
	big temp2; 	
	temp2 = mirvar(0);
	copy(pkdata[data], temp2);
	xgcd(temp2, pkp.p, temp2, temp2, temp2);
	mad(c, temp2, temp2, pkp.p, pkp.p, pkdata[1-data]);
	gettimeofday(&t1, 0);
	ofchooser += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
PK0 = pkdata[0];
PK1 = pkdata[1];
	// chooser sends pkdata[0] to sender

	// this part is computing by sender
	gettimeofday(&t0, 0);
	big pk0r, pk1r, temp3;
	pk0r = mirvar(0);
	pk1r = mirvar(0);
	temp3 = mirvar(0);
	powmod(pkdata[0], rc, pkp.p, pk0r);
	copy(pk0r, temp3);
	xgcd(temp3, pkp.p, temp3, temp3, temp3);
	mad(cr, temp3, temp3, pkp.p, pkp.p, pk1r);
	
	big h0, h1;
	h0 = mirvar(0);
	h1 = mirvar(0);
  //   gettimeofday(&t0, 0);
	h0 = HashP(pk0r, 0);
//     gettimeofday(&t1, 0);
    //    fattaneh += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	h1 = HashP(pk1r, 1);

	big xor[2];
	xor[0] = mirvar(0);
	xor[1] = mirvar(0);
	xor[0] = XOR(128, h0, m0);
	xor[1] = XOR(128, h1, m1);
	gettimeofday(&t1, 0);
	onsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// sender sends gr, xor0, and xor1 to chooser

	// this part is computing by chooser
	gettimeofday(&t0, 0);
	big hdata, temp4;
	hdata = mirvar(0);
	temp4 = mirvar(0);
	powmod(gr, k, pkp.p, temp4);
	hdata = HashP(temp4, data);
	result = XOR(128, hdata, xor[data]);
	gettimeofday(&t1, 0);
	onchooser += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	return result;
}

big HashP(big a1, int a2)
{
	big result;
	result = mirvar(0);
	char buffer[K+1];
    	bzero(buffer, K+1);
    	int len;
//	big temp;
//	temp = mirvar(a2);

	len = big_to_bytes(K, a1, buffer, TRUE);
buffer[K] = (char) a2;	
//	len = big_to_bytes(1, temp, buffer + sizeof(char) * K, TRUE);


	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < K+1; i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh, hash);
	
	char temp1[16];
	for (i = 0; i < 16; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(16, temp1, result);

	return result;
}

big XOR(int lin, big a1, big a2)
{
	big result;
	result = mirvar(0);
	int lint = (int)(ceil(lin/8));
	char buffer1[lint];
    	bzero(buffer1, lint);
	char buffer2[lint];
    	bzero(buffer2, lint);
	char buffer3[lint];
    	bzero(buffer3, lint);
    	int len;

	len = big_to_bytes(lint, a1, buffer1, FALSE);
	len = big_to_bytes(lint, a2, buffer2, FALSE);

	int i;
	for (i = 0; i < lint; i++)
		buffer3[i] = buffer1[i] ^ buffer2[i];

	bytes_to_big(lint, buffer3, result);

	return result;
}

int ZK(big c, int data)
{
	int result;
	// this part is computed by PR
gettimeofday(&t0, 0);
	big  ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pkc.n, temp1, ub);
	
	big v1;
	v1 = mirvar(0);
	strong_bigrand(&rng, ub, v1);
	
	big v2;
	v2 = mirvar(0);
	strong_bigrand(&rng, ub, v2);

	big w;
	w = mirvar(0);
	expb2(kk, ub);
	strong_bigrand(&rng, ub, w);
	
	big u1, u2;
	u1 = mirvar(0);
	u2 = mirvar(0);
	strong_bigrand(&rng, pkp.q, u1);
	strong_bigrand(&rng, pkp.q, u2);

	big temp5;
	temp5 = mirvar(0);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.g, temp5);
	xgcd(temp5, pkc.n, temp5, temp5, temp5);
gettimeofday(&t0, 0);
	mad(temp5, c, c, pkc.n, pkc.n, temp5);

	big t1t, t2, c1, c2, r1, r2, cp, s1, s2, z1, z2;
	big sub, one;
	sub = mirvar(0);
	one = mirvar(1);

big zero;
zero = mirvar(0);
	if (data == 1)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(c, w, pkc.n, temp4);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.g, v1, pkc.n, t1t);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp4, t1t, t1t, pkc.n, pkc.n, t1t);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.g, v2, pkc.n, t2);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);

		big temp44;
		s1 = mirvar(0);
		s2 = mirvar(0);
		temp44 = mirvar(0);
		powmod(PK0, w, pkp.p, temp44);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkp.g, u1, pkp.p, s1);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp44, s1, s1, pkp.p, pkp.p, s1);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkp.g, u2, pkp.p, s2);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		big ch = HashZKK(pkc.h, c, temp5, t1t, t2, s1, s2);
	
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c1 = w;
		subtract(zero, c1, sub);
		//add(sub, ch, sub);
		//powmod(sub, one, ub, c2);
mad(one, sub, ch, ub, ub, c2);		
r1 = v1;
		multiply(c2, rx, temp6);
		subtract(v2, temp6, r2);

		big temp66;
		z1 = mirvar(0);
		z2 = mirvar(0);
		temp66 = mirvar(0);
		z1 = u1;
		mad(c2, khat, khat, pkp.q, pkp.q, z2);
		subtract(zero, z2, sub);
		//add(sub, u2, sub);
		//powmod(sub, one, pkp.q, z2);
		mad(one, sub, u2, pkp.q, pkp.q, z2);

		// PR sends t1, t2, c, c1, c2, r1, r2, s1, s2, z1, z2 tp PS
	}
	if (data == 0)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(temp5, w, pkc.n, temp4);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.g, v2, pkc.n, t2);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp4, t2, t2, pkc.n, pkc.n, t2);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.g, v2, pkc.n, t2);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);

		big temp44;
		s1 = mirvar(0);
		s2 = mirvar(0);
		temp44 = mirvar(0);
		powmod(PK1, w, pkp.p, temp44);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkp.g, u2, pkp.p, s2);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp44, s2, s2, pkp.p, pkp.p, s2);
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkp.g, u1, pkp.p, s1);
gettimeofday(&t1, 0);
ZKK1ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);

		big ch = HashZKK(pkc.h, c, temp5, t1t, t2, s1, s2);
		
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c2 = w;
		subtract(zero, c2, sub);
mad(one, sub, ch, ub, ub, c2);		
//add(sub, ch, sub);
//		powmod(sub, one, ub, c1);
		r2 = v2;
		multiply(c1, rx, temp6);
		subtract(v1, temp6, r1);

		big temp66;
		z1 = mirvar(0);
		z2 = mirvar(0);
		temp66 = mirvar(0);
		z2 = u2;
		mad(c1, khat, khat, pkp.q, pkp.q, z1);
		subtract(zero, z1, sub);
mad(one, sub, u1, pkp.q, pkp.q, z1);		
//add(sub, u1, sub);
//		powmod(sub, one, pkp.q, z1);

		// PR sends t1, t2, c, c1, c2, r1, r2, s1, s2, z1, z2 tp PS
	}
gettimeofday(&t1, 0);
ZKK1onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// this part is computed by PS
gettimeofday(&t0, 0);
	big t1p, t2p, temp7, s1p, s2p;
	t1p = mirvar(0);
	t2p = mirvar(0);
	temp7 = mirvar(0);
	s1p = mirvar(0);
	s2p = mirvar(0);
	powmod(c, c1, pkc.n, temp7);
	powmod(pkc.h, r1, pkc.n, t1p);
	mad(temp7, t1p, t1p, pkc.n, pkc.n, t1p);
	powmod(temp5, c2, pkc.n, temp7);
	powmod(pkc.h, r2, pkc.n, t2p);
	mad(temp7, t2p, t2p, pkc.n, pkc.n, t2p);
	powmod(PK0, c1, pkp.p, temp7);
	powmod(pkp.g, z1, pkp.p, s1p);
	mad(temp7, s1p, s1p, pkp.p, pkp.p, s1p);
	powmod(PK0, c2, pkp.p, temp7);
	powmod(pkp.g, z2, pkp.p, s2p);
	mad(temp7, s2p, s2p, pkp.p, pkp.p, s2p);
	
	big ch1;
	ch1 = mirvar(0);
	ch1 = HashZKK(pkc.h, c, temp5, t1p, t2p, s1p, s2p);

	int aa = mr_compare(t1t, t1p);
	int bb = mr_compare(t2, t2p);
	int cc = mr_compare(s1, s1p);
	int dd = mr_compare(s2, s2p);
	cp = mirvar(0);
mad(one, c1, c2, ub, ub, cp);
	//add(c1, c2, cp);
	//powmod(cp, one, ub, cp);
	int ee = mr_compare(cp, ch1);

	if(aa == 0 && bb == 0 && cc ==0 && dd == 0 && ee ==0)
		result = 1;
	else
		result = 0;
gettimeofday(&t1, 0);
ZKK1ons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return result;
}

big HashZKK(big a, big b, big c, big d, big e, big f, big h)
{
	big result;
	result = mirvar(0);
	
	char buffer[7*length];
    	bzero(buffer, 7*length);
    	int len;

	len = big_to_bytes(length, a, buffer, TRUE);
	len = big_to_bytes(length, b, buffer + sizeof(char) * length, TRUE);
	len = big_to_bytes(length, c, buffer + sizeof(char) * 2*length, TRUE);
	len = big_to_bytes(length, d, buffer + sizeof(char) * 3*length, TRUE);
	len = big_to_bytes(length, e, buffer + sizeof(char) * 4*length, TRUE);
	len = big_to_bytes(length, h, buffer + sizeof(char) * 5*length, TRUE);
	len = big_to_bytes(length, f, buffer + sizeof(char) * 6*length, TRUE);

	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);   
   	int i;
    	for(i = 0; i < (5*length); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh , hash);
	
	char temp1[16];
	for (i = 0; i < 16; i++)
		temp1[i] = hash[i];
	
	bytes_to_big(16, temp1, result);

	return result;
}
int ZK1(big c, int data, big m0, big m1)
{
	int result;
	big res1, res2;
	res1 = mirvar(0);
	res2 = mirvar(0);
	//powmod(pkc.g, m0, pkc.n, res1);
	//powmod(pkc.h, rx, pkc.n, PK1);
	//mad(res1, PK0, PK0, pkc.n, pkc.n, PK0);
gettimeofday(&t0, 0);
	powmod(pkc.h, rx, pkc.n, PK1);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
	powmod(pkc.g, m1, pkc.n, res2);
	mad(res2, PK1, PK1, pkc.n, pkc.n, res2);
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// this part is computed by PR
gettimeofday(&t0, 0);
	big  ub, temp1;
	ub = mirvar(1);
	temp1 = mirvar(0);
	long int temp2 = kk+kp-1;
	expb2 (temp2, temp1);
	multiply(pkc.n, temp1, ub);
	
	big v1;
	v1 = mirvar(0);
	strong_bigrand(&rng, ub, v1);
	
	big v2;
	v2 = mirvar(0);
	strong_bigrand(&rng, ub, v2);

	big u1, u2;
	u1 = mirvar(0);
	u2 = mirvar(0);
	strong_bigrand(&rng, ub, u1);
	strong_bigrand(&rng, ub, u2);

	big w;
	w = mirvar(0);
	expb2(kk, ub);
	strong_bigrand(&rng, ub, w);

	big temp5;
	temp5 = mirvar(0);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	copy(pkc.g, temp5);
	xgcd(temp5, pkc.n, temp5, temp5, temp5);
gettimeofday(&t0, 0);
powmod(temp5, m0, pkc.n, PK0);
mad(PK0, res2, res2, pkc.n, pkc.n, PK0);
	mad(temp5, c, c, pkc.n, pkc.n, temp5);

	big t1t, t2, c1, c2, r1, r2, cp, s1, s2, z1, z2;
	big sub, one, zero;
	sub = mirvar(0);
	one = mirvar(1);
zero = mirvar(0);

	if (data == 1)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(c, w, pkc.n, temp4);
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.h, v1, pkc.n, t1t);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp4, t1t, t1t, pkc.n, pkc.n, t1t);
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.h, v2, pkc.n, t2);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		big temp44;
		s1 = mirvar(0);
		s2 = mirvar(0);
		temp44 = mirvar(0);
		powmod(PK0, w, pkc.n, temp44);
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.h, u1, pkc.n, s1);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		mad(temp44, s1, s1, pkc.n,pkc.n, s1);
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		powmod(pkc.h, u2, pkc.n, s2);
gettimeofday(&t1, 0);
ZKK2ofc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
gettimeofday(&t0, 0);
		big ch = HashZKK(pkc.h, c, temp5, t1t, t2, s1, s2);
		
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c1 = w;
		subtract(zero, c1, sub);
mad(one, sub, ch, ub, ub, c2);
		//add(sub, ch, sub);
		//powmod(sub, one, ub, c2);
		r1 = v1;
		multiply(c2, rx, temp6);
		subtract(v2, temp6, r2);

		big temp66;
		z1 = mirvar(0);
		z2 = mirvar(0);
		temp66 = mirvar(0);
		z1 = u1;
		mad(c2, rx, rx, pkc.n, pkc.n, z2);
		subtract(zero, z2, sub);
mad(one, sub, u2, ub, ub, z2); 
		//add(sub, u2, sub);
		//powmod(sub, one, ub, z2);

		// PR sends t1, t2, c, c1, c2, r1, r2, s1, s2, z1, z2 tp PS
	}
	if (data == 0)
	{
		big temp4;
		t1t = mirvar(0);
		t2 = mirvar(0);
		temp4 = mirvar(0);
		powmod(temp5, w, pkc.n, temp4);
		powmod(pkc.h, v2, pkc.n, t2);
		mad(temp4, t2, t2, pkc.n, pkc.n, t2);
		powmod(pkc.h, v2, pkc.n, t2);

		big temp44;
		s1 = mirvar(0);
		s2 = mirvar(0);
		temp44 = mirvar(0);
		powmod(PK1, w, pkc.n, temp44);
		powmod(pkc.h, u2, pkc.n, s2);
		mad(temp44, s2, s2, pkc.n, pkc.n, s2);
		powmod(pkc.h, u1, pkc.n, s1);

		big ch = HashZKK(pkc.h, c, temp5, t1t, t2, s1, s2);
		
		big temp6;
		c1 = mirvar(0);
		c2 = mirvar(0);
		r1 = mirvar(0);
		r2 = mirvar(0);
		temp6 = mirvar(0);
		c2 = w;
		subtract(ub, c2, sub);
		add(sub, ch, sub);
		powmod(sub, one, ub, c1);
		r2 = v2;
		multiply(c1, rx, temp6);
		subtract(v1, temp6, r1);

		big temp66;
		z1 = mirvar(0);
		z2 = mirvar(0);
		temp66 = mirvar(0);
		z2 = u2;
		mad(c1, rx, rx, pkc.n, pkc.n, z1);
		subtract(ub, z1, sub);
		add(sub, u1, sub);
		powmod(sub, one, ub, z1);

		// PR sends t1, t2, c, c1, c2, r1, r2, s1, s2, z1, z2 tp PS
	}
gettimeofday(&t1, 0);
ZKK2onc += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// this part is computed by PS
gettimeofday(&t0, 0);
	big t1p, t2p, temp7, s1p, s2p;
	t1p = mirvar(0);
	t2p = mirvar(0);
	temp7 = mirvar(0);
	s1p = mirvar(0);
	s2p = mirvar(0);
	powmod(c, c1, pkc.n, temp7);
	powmod(pkc.h, r1, pkc.n, t1p);
	mad(temp7, t1p, t1p, pkc.n, pkc.n, t1p);
	powmod(temp5, c2, pkc.n, temp7);
	powmod(pkc.h, r2, pkc.n, t2p);
	mad(temp7, t2p, t2p, pkc.n, pkc.n, t2p);
	powmod(PK0, c1, pkc.n, temp7);
	powmod(pkc.h, z1, pkc.n, s1p);
	mad(temp7, s1p, s1p, pkc.n, pkc.n, s1p);
	powmod(PK0, c2, pkc.n, temp7);
	powmod(pkc.h, z2, pkc.n, s2p);
	mad(temp7, s2p, s2p, pkc.n, pkc.n, s2p);
	
	big ch1;
	ch1 = mirvar(0);
	ch1 = HashZKK(pkc.h, c, temp5, t1p, t2p, s1p, s2p);

	int aa = mr_compare(t1t, t1p);
	int bb = mr_compare(t2, t2p);
	int cc = mr_compare(s1, s1p);
	int dd = mr_compare(s2, s2p);
	cp = mirvar(0);
mad(one, c1, c2, ub, ub, cp);
	//add(c1, c2, cp);
	//powmod(cp, one, ub, cp);
	int ee = mr_compare(cp, ch1);

	if(aa == 0 && bb == 0 && cc ==0 && dd == 0 && ee ==0)
		result = 1;
	else
		result = 0;
gettimeofday(&t1, 0);
ZKK2ons += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return result;
}

//******************************************************
char* XORInt(int lin, big a1, int *a2)
{
	int lint = (int)(floor(lin/8));
	char *result = malloc(sizeof(char)*lint);
	char buffer1[lint];
    	bzero(buffer1, lint);
	char buffer2[lint];
    	bzero(buffer2, lint);
    	int len;
	len = big_to_bytes(lint, a1, buffer1, FALSE);

	int i;
	int k = 0;
	for (i = 0; i < lint; i++)
		buffer2[i] = (char) (a2[8*i+7]+2*a2[8*i+6]+4*a2[8*i+5]+8*a2[8*i+4]+16*a2[8*i+3]+32*a2[8*i+2]+64*a2[8*i+1]+128*a2[8*i]);
	for (i = 0; i < lint; i++)
		result[i] = buffer1[i] ^ buffer2[i];


	return result;
}

char* XORByte(int lin, char *buffer1, char *buffer2)
{
	int lint = (int)(floor(lin/8));
	char *result = malloc(sizeof(char)*lint);
	//char buffer1[lint];
    	//bzero(buffer1, lint);
    	//int len;
	//len = big_to_bytes(lint, ai1, buffer1, FALSE);

	int i;
	for (i = 0; i < lint; i++)
		result[i] = buffer1[i] ^ buffer2[i];


	return result;
}

char* Gfunction(int lin, int lout, big kG)
{
	char *resultG = malloc(sizeof(char)*(lout));
	int len;
	len = big_to_bytes(lin/8, kG, numberG, FALSE);  
	strong_init(&rng1, lin , numberG, (mr_unsign32)PRG.tv_usec);
	int j = lout;
	int i = 0;
	big result;
	result = mirvar(0);
	while (j > 0)
	{
		if (j < 3*K)
		{
			strong_bigdig(&rng1, j, 2, result);
			len = big_to_bytes(j/8, result, resultG +(i*3*K), FALSE);
			j = 0;
		}
		else
		{
                        strong_bigdig(&rng1, 3*K, 2, result);
                        len = big_to_bytes(3*K/8, result, resultG+(i*3*K), FALSE);
                        j -= 3*K;
			i++;
		}
	}
	return resultG;
}

char* Hfunction(int lin, int lout, int jH, char *bH)
{
	char *resultH = malloc(128);
	char buffer[(int)(ceil(lin/8+24))];
	bzero(buffer, (int)(ceil(lin/8+24)));
    	int i, len;
	big temp;
	temp = mirvar(jH);

	for (len = 0; len < lin/8; len++)
		buffer[len] = bH[len];
	len = big_to_bytes(20, temp, buffer + sizeof(char) * (int)(ceil(lin/8)), TRUE);
		
	char hash[32];
	bzero(hash, 32);
	sha256 sh;   
	shs256_init(&sh);  
    	for(i = 0; i < (int)(ceil(lin/8+1)); i++)   
        	shs256_process(&sh,buffer[i]);   
	shs256_hash(&sh, hash);
	
	
	for (i = 0; i < 16; i++)
		resultH[i] = hash[i];
	
		
	return resultH;
}

big InttoBig(int lin, int *in)
{
	big result;
	result = mirvar(0);
	char buffer[lin/8];
	int i, j, temp;
	int k = lin-1;
	for (i = (lin/8)-1; i >= 0; i--)
	{
		temp = 0;
		for (j = 0; j < 8; j++)
		{
			temp+=pow(2, j)* in[k];
			k--;
		}
		buffer[i] = (char)temp;	
	}
	bytes_to_big(lin/8, buffer, result);

	return result;
}

char* InttoByte(int lin, int *in)
{
	char *result = malloc(sizeof(char)*(lin/8));
	int i, j, temp;
	int k = lin-1;
	for (i = (lin/8)-1; i >= 0; i--)
	{
		temp = 0;
		for (j = 0; j < 8; j++)
		{
			temp+=pow(2, j)* in[k];
			k--;
		}
		result[i] = (char)temp;	
	}

	return result;
}

int* BigtoInt(int lout, big in)
{
	int *result = malloc(sizeof(int)*lout);
	char buffer[lout/8];
	int i, j, len;
	len = big_to_bytes(lout/8, in, buffer, FALSE);
	int k = lout-1;
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < 8; j++)
		{
  			result[k] = (buffer[len-i-1] >> j) & 1;
			k--;
		}
	}

	return result;
}

int* BytetoInt(int lout, char *in)
{
	int *result = malloc(sizeof(int)*lout);
	int i, j, len;
	int k = lout-1;
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < 8; j++)
		{
  			result[k] = (in[len-i-1] >> j) & 1;
			k--;
		}
	}

	return result;
}

big * OTextention(int m, big *x0, big *x1, int *r)
{
	big *resultOT = malloc(128*m);
	int i, j, k;
	for (i = 0; i < m; i++)
		resultOT[i] = mirvar(0);

	// step 1

	// S choose a random string s
	gettimeofday(&t0, 0);

	big sr;
	sr = mirvar(0);
	strong_bigdig(&rng, kp, 2, sr);
	
	char *srb = malloc(kp);
	int len;
        len = big_to_bytes(kp/8, sr, srb, FALSE);
	int *stemp = malloc(sizeof(int)*kp);
	stemp = BytetoInt(kp, srb);


	gettimeofday(&t1, 0);
	ofchooser += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	//R chooses κ pairs of κ-bits seeds
	gettimeofday(&t0, 0);
	
	big kr0[kp];
	big kr1[kp];
	for (i = 0; i < kp; i++)
	{
		kr0[i] = mirvar(0);
		strong_bigdig(&rng, kp, 2, kr0[i]);
		kr1[i] = mirvar(0);
		strong_bigdig(&rng, kp, 2, kr1[i]);
	}
	gettimeofday(&t1, 0);
	ofsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	// step 2
	big rOT[kp];
	for (i = 0; i < kp; i++)
	{
		rOT[i] = mirvar(0);
		rOT[i] = OT(stemp[i], kr0[i], kr1[i]);
	}

	// step 4
	// R computes also S computes t1OT too
	gettimeofday(&t0, 0);
	
	char *rb = malloc(sizeof(char)*(m/8));
	char *ur[kp], *t0r[kp], *t1r[kp];
	for (j = 0; j < kp; j++)
	{
		ur[j] = malloc(sizeof(char)*(m/8));
		t0r[j] = malloc(sizeof(char)*(m/8));
		t1r[j] = malloc(sizeof(char)*(m/8));
	}
	
	rb = InttoByte(m, r);

	char *ttempr4 = malloc(sizeof(char)*(m/8));
	for (i = 0; i < kp; i++)
	{
		t0r[k] = Gfunction(kp, m, kr0[i]);
		t1r[k] = Gfunction(kp, m, kr1[i]);
		ttempr4 = XORByte(m, t1r[k], rb);
		ur[i] = XORByte(m, ttempr4, t0r[k]);
	}

	gettimeofday(&t1, 0);
	ofsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	// step 5

	// S defines
	gettimeofday(&t0, 0);

	char *ks = malloc(sizeof(char)*m/8);
	char *qs[kp];
	for (j = 0; j < kp; j++)
		qs[j] = malloc(sizeof(char)*(m/32));
	
	for (i = 0; i < kp; i++)
	{
		ks = Gfunction(kp, m, rOT[i]);
		if (stemp[i] == 0)
			qs[i] = Gfunction(kp, m, rOT[i]);
		else
		{
			ks = Gfunction(kp, m, rOT[i]);
			qs[i] = XORByte(m, ks, ur[i]);  
		}
	}
	// step 6
	
	// S sends

	int *qj = malloc(sizeof(int)*m*kp);
	int *qjs = malloc(sizeof(int)*m);
	for (i = 0; i < kp; i++)
	{
		qjs = BytetoInt(m, qs[i]);

		for (j = 0; j < m; j++)
			qj[i+kp*j] = qjs[j];
	}

        char *y60OT[m], *y61OT[m];
        for (j = 0; j < m; j++)
        {
                y60OT[j] = malloc(sizeof(char)*(kp/8));
                y61OT[j] = malloc(sizeof(char)*(kp/8));
        }

	int *temps6 = malloc(sizeof(int)*kp);
	char *temp4 = malloc(sizeof(char)*(kp/8));
	char *temp5 = malloc(sizeof(char)*(kp/8));
	char *temp44 = malloc(sizeof(char)*(kp/8));
	int *temp6 = malloc(sizeof(int)*(kp/8));
	char *temp7 = malloc(sizeof(char)*(kp/8));
        char *x0p = malloc(sizeof(char)*(kp/8));
        char *x1p = malloc(sizeof(char)*(kp/8));

	k = 0;
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < kp; j++)
		{
			temps6[j] = qj[k];
			k++;
		}
		temp4 = InttoByte(kp, temps6);
		temp5 = Hfunction(kp, lnew, i, temp4);
		temp44 = XORByte(kp, srb, temp4);
		temp7 = Hfunction(kp, lnew, i, temp44);
		len = big_to_bytes(kp/8, x0[i], x0p, FALSE);
		len = big_to_bytes(kp/8, x1[i], x1p, FALSE);
		y60OT[i] = XORByte(lnew, x0p, temp5); 
		y61OT[i] = XORByte(lnew, x1p, temp7);
	}

	gettimeofday(&t1, 0);
	onchooser += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	// step 7
	//
	// R computes
	gettimeofday(&t0, 0);

	int *tj = malloc(sizeof(int)*m*kp);
	int *tjr = malloc(sizeof(int)*m);
	k = 0;
	for (i = 0; i < kp; i++)
	{
		tjr = BytetoInt(m, t0r[k]);
		for (j = 0; j < m; j++)
			tj[i+kp*j] = tjr[j];

		k++;
	}

	gettimeofday(&t1, 0);
	ofsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
	
	int *tempr7 = malloc(sizeof(int)*kp);
	k = 0;

	gettimeofday(&t0, 0);
	
	for (i = 0; i < m; i++)
	{

		for (j = 0; j < kp; j++)
		{
			tempr7[j] = tj[k];
			k++;
		}
		temp4 = InttoByte(kp, tempr7);
		temp5 = Hfunction(kp, lnew, i, temp4);
	}

	gettimeofday(&t1, 0);
	ofsender +=(t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	gettimeofday(&t0, 0);
	for (i = 0; i < m; i++)
	{
		if (r[i] == 0)
			temp7 = XORByte(lnew, y60OT[i], temp5);
		else
			temp7 = XORByte(lnew, y61OT[i], temp5);
		bytes_to_big(16, temp7, resultOT[i]);		
	}

	gettimeofday(&t1, 0);
	onsender += (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;

	return resultOT;
}

