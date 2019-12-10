#include <stdio.h>
#include <stdbool.h>

// Prints the string passed to it
void print_boolean(char *s, bool a)
{
    if (a == true)
    {
        printf("%s: True\n", s) ;
    }
    else
    {
        printf("%s: False\n", s) ;
    }
    printf("-----------------------\n") ;
}

// Prints the value of the operands passed as two strings
void print_string(char *s0, char *s1)
{
    printf("%s: %s\n", s0, s1) ;
}


// Prints the value of the operands passed as two strings
void print_i32(char *s, int a)
{
    printf("%s: %d\n", s, a) ;
}


// Prints the value of the operands passed as two strings
void print_i64(char *s, long int a)
{
    printf("%s: %ld\n", s, a) ;
}

// Prints the value of the operands passed as two strings
void print_float(char *s, float a)
{
    printf("%s: %f\n", s, a) ;
}

// Prints the value of the operands passed as two strings
void print_double(char *s, double a)
{
    printf("%s: %f\n", s, a) ;
}