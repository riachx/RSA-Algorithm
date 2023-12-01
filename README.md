# Assignment 5

Generates RSA public + private key pairs and using the keys to encrypt and decrypt messages with the RSA algorithm.

This assignment includes three programs:

Keygen: Generates the RSA public and private key pairs
Encrypt: Encrypts files using the generated public key
Decrypt: Decrypts files using the generated private  key

## Build

    $ make

## Running

    $ ./keygen
    $ ./encrypt
    $ ./decrypt
    

## Cleaning

    $ make clean
    
## Files


#### keygen.c

Implementation and main() function for keygen program. Generates the public and private keys and a user's signature.

Accepts the following commands:
 
b: specifies min bits needed for public modulus n
i: specifies num of Miller-Rabin iterations to test primes
n: specifies public key file - default: rsa.pub
d: specifies private key file - default: rsa.priv
s: specifies random seed for random state init
v: enables verbose output
h: displays program manual


#### encrypt.c

Implementation and main() function for encrypt program. Accepts a file to be encrypted and uses public key to encrypt it into the output file. 

Accepts the following commands:
 
i: specifies input file to encrypt - default: stdin
o: specifies output file to encrypt - default: stdout
n: specifies public key file - default: rsa.pub
v: enables verbose output
h: displays program manual



#### decrypt.c

Implementation and main() function for decrypt program. Accepts a file to be decrypted and uses private key to decrypt it into the output file. 

Accepts the following commands:
 
i: specifies input file to decrypt - deafult: stdin
o: specifies output file to decrypt - default: stdout
n: specifies private key file - default: rsa.priv
v: enables verbose output
h: displays program manual

#### rsa.c

Implements RSA library with twelve different functions to generate keys, encrypt/decrypt files, producing signatures, and verify them.

#### rsa.h

Header for rsa.c

#### numtheory.c

Implements five different functions to compute the greatest common divisor, the modulo inverse, power mod, and prime numbers. Make_prime generates random numbers and utilizes is_prime to test them if they are prime or not. 

#### numtheory.h

Header for numtheory.c

#### randstate.c

Implements random state interface for RSA library and number theory functions

#### randstate.h

Header for randstate.c

#### Makefile

Makefile contains the commands to compile and clean the files.

#### DESIGN.pdf

Contains the design for the code.

#### WRITEUP.pdf

Contains the writeup for the code.


