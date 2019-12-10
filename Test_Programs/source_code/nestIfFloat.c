#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
    float input = atof(argv[1]);
    float x=0;
    if(input>0){
        if(input>5){
            x = input/5;
        }
        else{
            x = input+5;
        }
    }
    else{
        if(input < -5){
            x = -input/5;
        }
        else{
            x = -input+5;
        }
    }

    return x;
}