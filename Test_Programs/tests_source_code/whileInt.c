#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
    int input = atoi(argv[1]);
    int x=0;

    while(x < input)
    {
        x += 1;
    }

    return x;
}
