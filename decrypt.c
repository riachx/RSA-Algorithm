
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

    fprintf(stderr, "Usage: ./decrypt [options]\n");
    fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
    fprintf(stderr, "  writing the result to the specified output file.\n");
    fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
    fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
    fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
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
    char rsa_priv[] = "rsa.priv";
    int in = false;
    int out = false;

    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1)
    {
        switch (opt)
        {
        // specifies input to decrypt
        case 'i':
            in = true;
            strcpy(infile, optarg);
            break;
        // specifies output to decrypt
        case 'o':
            out = true;
            strcpy(outfile, optarg);
            break;
        // specifies file containing public key
        case 'n':
            strcpy(rsa_priv, optarg);
            break;
        // enables verbose output
        case 'v':
            verbose = true;
            break;
        case 'h':
            print_help();
            return 0;
            break;
        default:
            print_help();
            return 0;
            break;
        }
    }

    FILE *rsapriv = fopen(rsa_priv, "r");
    if (!rsapriv)
    {
        fprintf(stderr, "Error opening public file.");
        return 1;
    }

    FILE *input = stdin;
    FILE *output = stdout;

    mpz_t n;
    mpz_t d;
    mpz_init(d);
    mpz_init(n);

    if (in == true)
    {
        input = fopen(infile, "r");
    }
    if (out == true)
    {
        output = fopen(outfile, "w");
    }
    if (!input)
    {
        fprintf(stderr, "Error opening input file.");
        return 1;
    }
    if (!output)
    {
        fprintf(stderr, "Error opening output file.");
        return 1;
    }

    rsa_read_priv(n, d, rsapriv);

    if (verbose == true)
    {
        gmp_fprintf(stderr, "n - modulus (%d bits): %Zd\nd- private exponent (%d bits): %Zd\n", mpz_sizeinbase(n,2), n, mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(input, output, n, d);
    
    fclose(rsapriv);

    mpz_clear(d);
    mpz_clear(n);

    return 1;

}
