#include "randstate.h"
#include <stdlib.h>


void randstate_init(uint64_t seed){
    
    gmp_randinit_mt(state);
    srandom(seed);
    
    gmp_randseed_ui(state,seed);

}

//
void randstate_clear(void){
    gmp_randclear(state);

}

