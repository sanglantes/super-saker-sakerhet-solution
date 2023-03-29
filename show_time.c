#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    srand(strtoul(argv[1], NULL, 10));
    unsigned long long password = 1;

    for (int i = 0; i < 5; i++) {
        password *= rand();
    }

    printf("%llu\n", password);
    return 0;
}
