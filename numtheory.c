#include "numtheory.h"
#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include "randstate.h"

void mod_inverse(mpz_t o, mpz_t a, mpz_t n){
  // assign n to r1

  mpz_t r1;
  mpz_init(r1);
  mpz_set(r1,n);
  
  // assign a to r2
  mpz_t r2;
  mpz_init(r2);
  mpz_set(r2,a);

  //assign 0 to t1
  mpz_t t1;
  mpz_init_set_ui(t1,0);
  //asign 1 to t2
  mpz_t t2;
  mpz_init_set_ui(t2,1);

  mpz_t q;
  mpz_init(q);

  mpz_t tmp_r1;
  mpz_init(tmp_r1); 
   
  mpz_t tmp_t1;
  mpz_init(tmp_t1); 

  mpz_t m1;
  mpz_init(m1); 

  mpz_t m2;
  mpz_init(m2);
  
  while(mpz_cmp_ui(r2,0) != 0){

     mpz_fdiv_q(q,r1,r2); // q = r/r'
     
     mpz_set(tmp_r1,r1);  //tmp = r

     mpz_set(r1,r2); //r = r'
     mpz_mul(m1,q,r2);   // r2 = r1-q*r2
     mpz_sub(m1,tmp_r1,m1);
     mpz_set(r2,m1);

     mpz_set(tmp_t1,t1); 
     mpz_set(t1,t2); // t = t'
     mpz_mul(m2,q,t2);  // m2 = q*t'
     mpz_sub(m2,tmp_t1,m2); // m2 = t-m2
     mpz_set(t2,m2);  // t' = m2
   
  }
  mpz_clear(tmp_t1);
  mpz_clear(tmp_r1);
  mpz_clear(q);
  mpz_clear(m1);
  mpz_clear(m2);
  mpz_clear(t2);
  mpz_clear(r2);
  
  if(mpz_cmp_ui(r1,1) > 0){

     mpz_clear(r1);
     mpz_clear(t1);
     
     return;
  }
  if(mpz_cmp_ui(t1,0) < 0){
     mpz_add(t1,t1,n);
     mpz_set(o,t1);
     
     
  }
mpz_set(o,t1);
mpz_clear(t1);
mpz_clear(r1);

}

void gcd(mpz_t d, mpz_t a, mpz_t b){
  mpz_t t;
  mpz_init(t);
  mpz_t b2;
  mpz_init(b2);
  mpz_set(b2,b);
  while(mpz_cmp_ui(b2,0) != 0){
     mpz_set(t,b2);
     mpz_mod (b2,a,b2);
     mpz_set(a,t);
  }
  mpz_set(d,a);

  mpz_clear(t);
  mpz_clear(b2);
  
}


//output, base, exponent, modulus
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n){
 
    mpz_t v;
    mpz_init(v);
    mpz_set_ui(v,1);
    
    mpz_t p;
    mpz_init(p);
    mpz_set(p,a);

    mpz_t d2;
    mpz_init(d2);
    mpz_set(d2,d);
    
    while ( mpz_cmp_ui(d2,0) > 0){

          if(mpz_odd_p(d2) != 0){
            mpz_mul(v,v,p);
            mpz_mod (v,v,n);
            //v = (v * p) % n;
        }
        mpz_mul(p,p,p);
        mpz_mod (p,p,n);
        mpz_fdiv_q_ui(d2, d2,2);
    }
    
    mpz_set(o,v);
    mpz_clear(v);
    mpz_clear(d2);
    mpz_clear(p);
    
}

bool is_prime(mpz_t n, uint64_t iters){

  //test for small base cases 
  if(mpz_cmp_ui(n,2) == 0 || mpz_cmp_ui(n,3) == 0 || mpz_cmp_ui(n,5) == 0 ){
    return true;
  }
  if(mpz_cmp_ui(n,1) <= 0){
    return false; //return not prime if less than 2.
  }
  //initialize s
  mpz_t s;
  mpz_init(s);

  //initialize r
  mpz_t r;
  mpz_init(r);
  //m = n-1;
  mpz_t m;
  mpz_init(m);
  mpz_sub_ui(m, n,1);

  mpz_t y;
  mpz_init(y);  
  
  mpz_t j;
  mpz_init(j);

  mpz_t a;  
  mpz_init(a);
  
  mpz_t n_2;
  mpz_init(n_2);
  mpz_sub_ui(n_2,n,2);
  
  mpz_t two;
  mpz_init_set_ui(two,2); 

  mpz_t n_1;
  mpz_init(n_1);
  mpz_sub_ui(n_1,n,1);
  
  mpz_t n_3;
  mpz_init(n_3);
  

  //dividing until odd
  while( mpz_divisible_ui_p(m,2) != 0){ //if divisible by 2, returns non zero
    //m = m/2;
    mpz_fdiv_q_ui(m,m,2);
    mpz_add_ui(s,s,1);
    //s++;
  }

  mpz_t s_1;
  mpz_init(s_1);
  mpz_sub_ui(s_1,s,1);

  mpz_set(r,m);

  //r = m / 2^s ;

    for(uint64_t i = 0; i < iters; i+=1){

        mpz_sub_ui(n_3,n,3); //subtract 3 to make next line interval 0 to n-3
        mpz_urandomm(a,state,n_3); // remember urandomm generates (0, n-1)
        mpz_add_ui(a,a,2); // add 2 to change to (2, n-1)
        
        //if(mpz_cmp_ui(a,2) >= 0 && mpz_cmp(a,n_2) <= 0){
          
          pow_mod(y,a,r,n); 
          mpz_sub_ui(n_3,n,1); 
          
          if(mpz_cmp_ui(y,1) != 0 && mpz_cmp(y,n_1) != 0){
            mpz_set_ui(j,1);
            while(mpz_cmp(j,s_1) <= 0 && mpz_cmp(y,n_1) != 0){
              
                pow_mod(y,y,two,n);
                
                if (mpz_cmp_ui(y,1)== 0){
                  mpz_clears(s,r,m,y,j,a,s_1,two,n_1,n_3,NULL);
                  return false;
                }
                mpz_add_ui(j,j,1); //incrementing j per loop
            
            }
            if(mpz_cmp(y,n_1) != 0){
              mpz_clears(s,r,m,y,j,a,s_1,two,n_1,n_3,NULL);
              return false;
            }
      }
    }
    mpz_clears(y,j,a,s,r,s_1,m,two,n_1,n_3,NULL);
    return true;
}


void make_prime(mpz_t p, uint64_t bits, uint64_t iters){
  
  mpz_t num;
  mpz_init(num); 
  while(1){
    mpz_urandomb(num,state,bits); // urandomb uses 0 to 2^(n) -1
    
    if(mpz_sizeinbase(num,2)>=bits){ //ensure num of bits is acceptable

      if(mpz_even_p(num) != true &&  mpz_cmp_ui(num,2)>0){
        
        if(is_prime(num,iters) == true){
          mpz_set(p,num);
          mpz_clear(num);
          return; //when finding a prime, return
        }
      }
    }
  }
  mpz_clear(num);
}
