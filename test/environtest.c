#include <stdio.h>
extern char **environ;

int main(int argc, char **argv)
{
	int len = 0;
	while (environ[len]){
		len++;
	}
	for (int i = 0; i < len; i++)
	{
		printf("%s\n", environ[i]);
	}	
}