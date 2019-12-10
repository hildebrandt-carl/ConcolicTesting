#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
    int input = atoi(argv[1]);
    int x=0;
    if(input>0)
    {
        if(input > 5)
        {
            if(input < 3)
            {
                x = 2;
            }
        }
        else
        {
            x = 1;
        }
    }
    else
    {
        x = 3;
    }

    return x;
}