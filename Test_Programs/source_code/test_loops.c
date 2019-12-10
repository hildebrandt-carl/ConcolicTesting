# include <stdio.h>

int main()
{
	int c = 0 ;
	for (unsigned int i = 0; i < 10; i++)
	{
		c = c + 1 ;
	}
	printf("%d",c) ;

	while(c > 10)
	{
		c++ ; 
	}
	printf("%d",c) ;

	return -1 ;
}


