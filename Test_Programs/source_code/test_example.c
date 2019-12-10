# include <stdio.h>

int add(int num1, int num2);

int main()
{
	int a = 1 ;
	int b = 2 ;
	int c ;
	c = add(a,b) ;
	return -1 ;
}

int add(int num1, int num2)
{
	int result ;
	result = num1 + num2 ; 
	if (result > 10)
	{
		result = 5 ;
	}
	return result ;
}