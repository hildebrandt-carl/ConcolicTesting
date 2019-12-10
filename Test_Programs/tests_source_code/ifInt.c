#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
    int input = atoi(argv[1]);
    int x=0;
    if(input>0){
        x = input;
    }
    else{
        x = -input;
    }

    return x;
}