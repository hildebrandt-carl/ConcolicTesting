# include <stdio.h>

int add(int num1, int num2);
int max(int num1, int num2);
int multiply(int num1, int num2);

int main()
{
	int a = 1 ;
	int b = 2 ;
	int c ;
	c = max(a,b) ;
	printf("Add: %d\n",c) ;
	c = multiply(a,b) ;
	printf("Multiply: %d\n",c) ;
	c = add(a,b) ;
	return -1 ;
}

int add(int num1, int num2)
{
	int result ;
	result = num1 + num2 ; 
	return result ;
}

int max(int num1, int num2)
{
	int result ;
	
	if (num1 > num2)
	{
		result = num1 ;
	}
	else
	{
		result = num2 ;
	}
	
	return result ;
}

int multiply(int num1, int num2)
{
	int result = 0;

	for (int i = 0; i < num2; i++)
	{
		result = add(result, num1) ;
	}

	return result ;
}