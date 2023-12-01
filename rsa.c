#include "rsa.h"
#include "numtheory.h"
#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>  
#include "randstate.h"
#include <stdbool.h>

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters){
        
    uint64_t p_bits;
    uint64_t q_bits;
    
    //(upper - lower +1) + lower
    uint64_t rand_num = random() % ((3*nbits)/4 - (nbits/4) + 1) + nbits/4;
    p_bits = rand_num;
    q_bits = nbits - p_bits;
    
    
    //rand_num = p_bits + q_bits + iters;
    //call make prime to generate random p and q values based on the random pbits and qbits we generated
    make_prime(p,p_bits,iters);
    make_prime(q,q_bits,iters);
    
    
    mpz_mul(n,p,q);
    
    mpz_t p_1;
    mpz_t q_1;
    mpz_t t;
    mpz_t t_1;
    mpz_t t_2;
    mpz_t g;
    mpz_t rand_num2;
    mpz_t t3;
    
    mpz_init(p_1);
    mpz_init(q_1);
    mpz_init(t);
    mpz_init(t_1);
    mpz_init(t_2);
    mpz_init(g);
    mpz_init(rand_num2);
    mpz_init(t3);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    mpz_sub_ui(p_1, p,1);
    mpz_sub_ui(q_1, q,1);

    mpz_mul(t_1,p_1, q_1);

    gcd(t_2,p_1,q_1);

    mpz_fdiv_q(t,t_1,t_2);
    
    //REMEMBER: totient = (p-1)(q-1) / gcd (p-1,q-1)
    //check if e is correct by ensuring gcd(e,totient) = 1
    
    while(1){  //finding e....

        mpz_urandomb(rand_num2, state, nbits);
        
        mpz_set(t3,rand_num2);

        gcd (g,rand_num2,t); 
        if (mpz_cmp_ui(g,1) == 0 && mpz_cmp_ui(t3,2)>0 && mpz_cmp(t3,n)<0) //make sure e greater than 2 and less than n
        {
            
            mpz_set(e,t3);
            break;
        }


    }
    mpz_clear(g);
    mpz_clear(t);
    mpz_clear(t_2);
    mpz_clear(t_1);       
    mpz_clear(p_1);                                                        
    mpz_clear(q_1);
    mpz_clear(rand_num2);
    return;

}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){

    if(pbfile == NULL)
       {
          printf("Error writing to public file!");   
          exit(1);             
       }

    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n,e,s,username);
   
}
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){

     if(pbfile == NULL)
       {
          printf("Error reading public file!");   
          exit(1);             
       }
    fseek(pbfile,0, SEEK_SET); //jump to beginning of file
    gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n,e,s,username);

}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q){

    mpz_t t_1;
    mpz_t p_1;
    mpz_t q_1;
    mpz_t t_2;
    mpz_t t;

    mpz_init(t_1);
    mpz_init(p_1);
    mpz_sub_ui(p_1,p,1);
    mpz_init(q_1);
    mpz_sub_ui(q_1,q,1);
    mpz_init(t_2);
    mpz_init(t);

    mpz_mul(t_1, p_1, q_1);
    gcd(t_2,p_1,q_1);
    mpz_fdiv_q(t,t_1,t_2);
    mod_inverse(d,e,t);
	
    /*REMEMBER: totient = (p-1)(q-1) / gcd (p-1,q-1)
    * check if d is correct by:
    * calculating t
    * mod_inverse(e, t) == d
    */

    mpz_clear(t_1);
    mpz_clear(p_1);
    mpz_clear(q_1);
    mpz_clear(t_2);
    mpz_clear(t);
	
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){

    if(pvfile == NULL)
       {
          printf("Error writing to private file!");   
          exit(1);             
       }

   
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n,d);

}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile){

     if(pvfile == NULL)
       {
          printf("Error reading private file!");   
          exit(1);             
       }
    fseek(pvfile, 0, SEEK_SET);
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", n,d);

}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
    pow_mod(c,m,e,n);

}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e){
    fseek(infile,0,SEEK_SET);
    fseek(outfile,0,SEEK_SET);

    uint8_t k = ((mpz_sizeinbase(n,2))-1)/8;
    uint8_t *block  = (uint8_t *) malloc(k);
    mpz_t m;
    mpz_init(m);

    mpz_t c;
    mpz_init(c);
    
    if ( infile == NULL ){
        printf("Couldn't read input.\n");
        return;
    }
     if ( outfile == NULL ){
        printf("Couldn't write to output\n");
        return;
    }

    int read = 0;
    block[0]= 0xFF;

    while(( read = fread(block+1, 1,k-1, infile)) > 0)
    {
        mpz_import (m, read+1, 1, sizeof(uint8_t), 1, 0, block);
        rsa_encrypt(c,m,e,n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    
    fclose(infile);
    
    mpz_clears(m,c,NULL);
    free(block);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n){
    pow_mod(m,c,d,n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d){

    uint8_t k = ((mpz_sizeinbase(n,2))-1)/8;
    uint8_t *block  = (uint8_t *) malloc(k);
    
    mpz_t m;
    mpz_init(m);

    mpz_t c;
    mpz_init(c);

    if ( infile == NULL ){
        printf("Couldn't read input.\n");
        return;
    }
     if ( outfile == NULL ){
        printf("Couldn't write to output\n");
        return;
    }
    //scanf
    while(gmp_fscanf(infile, "%Zx\n",c)!=EOF){//or print?
            rsa_decrypt(m,c,d,n);
            size_t j = 0;
            mpz_export(block, &j,1,1,1,0,m);
            fwrite(block+1,1,j-1,outfile);
        }
    
    fclose(infile);
    free(block);
}
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n){

    pow_mod(s,m,d,n);

}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n){

    mpz_t t;
    mpz_init(t);
    pow_mod(t,s,e,n);
    
    if(mpz_cmp(t,m)==0){
        mpz_clear(t);
        return true;
    }
     mpz_clear(t);
    return false;

//keygen generates pub and priv keys, 
//read one block at a time and decrypt
//
//export stores into j and array

}
