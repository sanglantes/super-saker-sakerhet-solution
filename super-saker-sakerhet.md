# Super Säker Säkerhet write-up

Super Säker Säkerhet was a 200-point challenge during the SSM 2023 qualifiers. It featured a C file and a server to connect to.

## Description
> Mitt lösenord är oknäckbart! Det är bokstavligen slumpmässigt!

Translated to English:
> My password is uncrackable! It is literally randomized!

**Author** Laith Abbawi<br>
**Files** main.c<br>
**Service** nc 35.217.50.247 50000<br>
**Unofficial difficulty** Easy
## Source code
```
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "flag.h"

int main()
{
    srand(time(NULL));
    unsigned long long password = 1;
    unsigned long long input = 0;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    for (int i = 0; i < 5; i++)
        password *= rand();

    puts("Välkommen! Var vänlig mata in lösenordet.");

    scanf("%llu", &input);
    if (input == password)
    {
        printf("Grattis! Flaggan är: %s\n", flag);
        return 0;
    }
}
```
## Analysis

The first step to the solution is understanding what the objective is, as well as understanding what the source code does.

### Lines of interest
**`srand(time(NULL))`**: According to the `srand()` man page, it sets a seed for `rand()`, a function that generates a random number in the interval [0, RAND_MAX]. RAND_MAX is an integer constant guaranteed to be larger than 32,767. `rand()` is dependent on a seed to be random. This means that the same seed will generate the same random number. In this case, the seed it set to the current time expressed in a UNIX timestamp.

**`password`** and **`input`**: `password` is set to `1` as it will be iteratively mulitplied with a random integer five times. `input` will be the password given by the user. If the randomly generated password and user-given password match, the flag is printed.

## Solution

Before anything, the random seed is chosen. This is good for us as we can wait as long as we'd like before entering the password. Also note that once the seed is chosen, it will remain the same for all five `password` iterations. 

If we know what `rand()` will output, we can easily figure out the password. We know what its output will be since we know what seed it uses.

**show_password.c**
```
#include <stdio.h>
#include <random.h>
int main(int argc, char* argv[]) {
    srand(strtoul(argv[1]));
    unsigned long long password = 1;
    
    for (int i = 0; i < 5; i++) {
        password *= rand();
    }
    
    printf("%llu\n", password);
    return 0;
}
```
This program works similarly to the original challenge application, with the difference being that the user controls the seed. The `strtoul()` function will simply convert a string of unsigned long integers into its numerical value.

After compiling this program, we can use a simple Python script that passes the current UNIX time like this:

`$ ./show_password "$(python3 -c 'from time import time; from math import floor; print(floor(time()))')"`

With some Bash magic, we can pipe the output from `show_password` to a netcat instance connecting to the challenge server like this:

`$ ./show_time "$(python3 -c 'from time import time; from math import floor; print(floor(time()))')" | nc 35.217.50.247 50000`
Välkommen! Var vänlig mata in lösenordet.
Grattis! Flaggan är: SSM{w0w_m1n_54k3rh37_v4r_1n73_71llr4ckl16}`

## Discussion

Whilst not being a difficult challenge, it provided some useful insights about C's built-in pseudo-random number generator. It is not uncommon for `rand()` to be used with a time-based seed. Remember that `rand()` is not a cryptographically secure number generator and should thus only be used in applications where predictability is not of importance. 

[Seth](https://crypto.stackexchange.com/questions/15662/how-vulnerable-is-the-c-rand-in-public-cryptography-protocols) from StackExchange's Cryptography network explains one of the fundamental cryptographic weaknesses in `rand()`,
> In the example you [referring to the OP] linked, the current time (specifically, a value representing the number of seconds elapsed since Jan 1, 1970 UTC) is used as the seed. If an attacker knows which year you generated your key, then that leaves only about 2^25 possible values for the seed --- and therefore only about 2^25 possible values for your key. At this point, he can guess your key pretty easily simply through brute force.

In case you're in need of CSPRNGs, use OpenSSL's C library. Alternatively, you could use GMP if you're seeking a more intimate connection with Linux' special file `/dev/urandom`. 
