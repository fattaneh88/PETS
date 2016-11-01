#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "COT.h"

int main()
{	

        initialize();

        //KeyGenerator();
        //store_key();
        read_key();
        int number = 117376;
        big *m0 = malloc(128*number);
        big  *m1 = malloc(128*number);
        int i;
        for (i = 0; i < number; i++)
        {
            m0[i] = mirvar(0);
            strong_bigdig(&rng, 128, 2, m0[i]);
            m1[i] = mirvar(0);
            strong_bigdig(&rng, 128, 2, m1[i]);
        }

        big *result = malloc(128*number);
        int *r = malloc(sizeof(int)*number);
        for (i = 0; i < number; i++)
        {
            result[i] = mirvar(0);
            if (i % 3 == 1)
                r[i] = 1;
            else
                r[i] = 0;
        }
    
        onsender = 0;
        onchooser = 0;
        ofsender = 0;
        ofchooser = 0;
	
        result = OTextention(number, m0, m1, r);

        printf("Offline sender:%lld\n", ofsender);
        printf("Offline chooser:%lld\n", ofchooser);
        printf("online sender:%lld\n", onsender);
        printf("online chooser:%lld\n", onchooser);

        return 0;
}


