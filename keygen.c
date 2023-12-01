/********************************
Your key generator program should accept the following command-line options:
• -b: specifies the minimum bits needed for the public modulus n.
• -i: specifies the number of Miller-Rabin iterations for testing primes (default: 50).
• -n pbfile: specifies the public key file (default: rsa.pub).
• -d pvfile: specifies the private key file (default: rsa.priv).
• -s: specifies the random seed for the random state initialization (default: the seconds since the
UN I X epoch, given by time(NULL)).
• -v: enables verbose output.
• -h: displays program synopsis and usage.
The program should follow these steps:
1. Parse command-line options using getopt() and handle them accordingly.
2. Open the public and private key files using fopen(). Print a helpful error and exit the program in
the event of failure.
3. Using fchmod() and fileno(), make sure that the private key file permissions are set to 0600,
indicating read and write permissions for the user, and no permissions for anyone else.
4. Initialize the random state using randstate_init(), using the set seed.
5. Make the public and private keys using rsa_make_pub() and rsa_make_priv(), respectively.
6. Get the current user’s name as a string. You will want to use getenv().

7. Convert the username into an mpz_t with mpz_set_str(), specifying the base as 62. Then, use
rsa_sign() to compute the signature of the username.
8. Write the computed public and private key to their respective files.
9. If verbose output is enabled print the following, each with a trailing newline, in order:
(a) username
(b) the signature s
(c) the first large prime p
(d) the second large prime q
(e) the public modulus n
(f ) the public exponent e
(g) the private key d
All of the mpz_t values should be printed with information about the number of bits that constitute
them, along with their respective values in decimal. See the reference key generator program for
an example.
10. Close the public and private key files, clear the random state with randstate_clear(), and clear
any mpz_t variables you may have u

*********************************/

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>

gmp_randstate_t state;

//function to print help message
int print_help(void){

     fprintf(stderr,"Usage: ./keygen [options]\n  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
     fprintf(stderr,"  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
     fprintf(stderr,"  the program options.\n");
     fprintf(stderr,"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
     fprintf(stderr,"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
     fprintf(stderr,"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
     fprintf(stderr,"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
     fprintf(stderr,"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
     fprintf(stderr,"    -v          : Enable verbose output.\n");
     fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
     return 0;
}

int main(int argc, char *argv[])
{  
    bool verbose = false;
    int opt = 0;
    int64_t bits = 1024;
    uint64_t iters = 50;
    uint64_t seed = time(NULL);
    //store default names of priv and public in a char
    char pbfile[] = "rsa.pub";
    char pvfile[] = "rsa.priv";

    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1)
    {
        switch (opt)
        {
        case 'b':
            bits = atoi(optarg);
            if (bits < 0 || bits > 504096){
                return 1;
            }
            break;
        case 'i':
            if (bits < 0 || bits > 1500){
                return 1;
            }
            iters = atoi(optarg);

            break;
        case 'n':
            strcpy(pbfile, optarg); // copy inputted file name into char of public file 
            break;
        case 'd':
            strcpy(pvfile, optarg); // copy inputted file name into char of private file 
            break;
        case 's':
            seed = atoi(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            print_help();
            return 0;
        }
    }

    FILE *public = fopen(pbfile, "r");
    FILE *private = fopen(pvfile, "r");
    
    if (!public)
    {
        fprintf(stderr, "Error inputting public file.");
        return 1;
    }
    if (!private)
    {
        fprintf(stderr, "Error inputting private file.");
        return 1;
    }

    int fb = fileno(public);
    if (fchmod(fb, S_IRUSR | S_IWUSR)!=0){
    	fprintf(stderr,"Permissions error!");
        return 1;  
    }
    
    int fv = fileno(private);
    if (fchmod(fv, S_IRUSR | S_IWUSR)!=0){
    	fprintf(stderr,"Permissions error!");
        return 1;  
    }
    
    randstate_init(seed);
    
    mpz_t d;
    mpz_t p;
    mpz_t q;
    mpz_t e;
    mpz_t n;
    mpz_t user;
    mpz_t s;
    mpz_t u;
    mpz_init(u);
    mpz_init(d);
    mpz_init(p);
    mpz_init(q);
    mpz_init(e);
    mpz_init(n);
    mpz_init(user);
    mpz_init(s);
  
    
    rsa_make_pub(p,q,n,e,bits,iters);
    rsa_make_priv(d,e,p,q);
    
    char *username = getenv("USER");
    
    mpz_set_str(u,username,62);
    rsa_sign(s,u,d,n);
    
    rsa_write_pub(n, e, s, username, public);
    rsa_write_priv(n, d, private);
    
    if(verbose == true){
       
        gmp_fprintf(stderr, "username %s\nuser signature: (%d bits): %Zd\np (%d bits): %Zd", username,mpz_sizeinbase(s,2), s, mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stderr, "\nq (%d bits): %Zd\nn - modulus (%d bits): %Zd\ne - public exponent (%d bits): %Zd\nd - private exponent (%d bits): %Zd\n",mpz_sizeinbase(q,2),q,mpz_sizeinbase(n,2),n,mpz_sizeinbase(e,2),e,mpz_sizeinbase(d,2),d);
        
    }
    
    randstate_clear();
    
    fclose(public);
    fclose(private);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(s);
    mpz_clear(u);
    mpz_clear(d);
    
    mpz_clear(user);
    return 1;
}
