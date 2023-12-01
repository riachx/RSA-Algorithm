
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

gmp_randstate_t state;

int print_help(void)
{
    fprintf(stderr, "Usage: ./encrypt [options]\n");
    fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
    fprintf(stderr, "  writing the result to the specified output file.\n");
    fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
    fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
    fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
    fprintf(stderr, "    -v          : Enable verbose output.\n");
    fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
    return 0;
}

int main(int argc, char *argv[])
{
    bool verbose = false;
    int opt = 0;
    char infile[128] = "stdin";
    char outfile[128] = "stdout";
    char rsa_pub[] = "rsa.pub";
    int in = false;
    int out = false;
    FILE *input;
    FILE *output;

    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1)
    {
        switch (opt)
        {
        // specifies input to encrypt
        case 'i':
            in = true;
            strcpy(infile, optarg);
            printf("file name = %s\n", infile);
            break;
        // specifies output to encrypt
        case 'o':
            out = true;
            strcpy(outfile, optarg);
            break;

        // specifies file containing public key
        case 'n':
            strcpy(rsa_pub, optarg);
            break;

        // enables verbose output
        case 'v':
            verbose = true;
            break;
        case 'h':
            print_help();
            return 0;
            //break;
        default:
            print_help();
            return 0;
            //break;
        }
    }

    FILE *rsapub = fopen(rsa_pub, "r");
    if (!rsapub)
    {
        fprintf(stderr, "Error opening public file.");
        return 1;
    }

    char *username = getenv("USER");
    

    mpz_t e;
    mpz_t n;
    mpz_t m;
    mpz_t s;
    mpz_t u;
    mpz_t d;
    mpz_init(d);
    mpz_init(e);
    mpz_init(n);
    mpz_init(m);
    mpz_init(s);
    mpz_init(u);
    
    
    if (in == false)
    {
        input = stdin;
    }
    if (out == false)
    {
        output = stdout;
    }

    if (in == true)
    {
        input = fopen(infile, "r");
        
    }
    if (out == true)
    {
        output = fopen(outfile, "w");
    }
    
    //printf("input name: %s\n",infile);
    if (input == NULL)
    {
        fprintf(stderr, "Error opening input file.");
        return 1;
    }


    if (output == NULL)
    {
        fprintf(stderr, "Error opening output file.");
        return 1;
    }

    rsa_read_pub(n, e, s, username, rsapub);

    if (verbose == true)
    {
        gmp_fprintf(stderr, "username %s\nuser signature: (%d bits): %Zd\nn - modulus (%d bits): %Zd\ne- public exponent (%d bits): %Zd\n", username,mpz_sizeinbase(s,2), s, mpz_sizeinbase(n, 2), n, mpz_sizeinbase(m, 2), m, mpz_sizeinbase(e, 2), e);
    }

    rsa_verify(m, s, e, n);

    rsa_encrypt_file(input, output, n, e);
    
    mpz_set_str(u, username, 62);

    fclose(rsapub);
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(m);
    mpz_clear(s);
    mpz_clear(u);


    return 1;



}
