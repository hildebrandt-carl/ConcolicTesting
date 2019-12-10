#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
    float input = atof(argv[1]);
    float x=0;
    if(input == 0){
        x = input;
    }
    else{
        x = -input;
    }

    return x;
}