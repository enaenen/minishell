#include <termios.h>
#include <unistd.h>
#include <stdio.h>

void ten_to_two(unsigned long n) {
    unsigned long a = 0x8000000000000000; // 1000 0000 0000 0000 0000 0000 0000 0000(2) <약 21억>
    for (int i = 0; i < 64; i++) {
        if ((a & n) == a)
            printf("1");
        else
            printf("0");
        a >>= 1;
    }
}

int	main(void)
{
	struct termios def_term;

	// printf(" %lx \n", def_term.c_iflag);
	// printf(" %lx \n", def_term.c_oflag);
	// printf(" %lx \n", def_term.c_cflag);
	// printf(" %lx \n", def_term.c_lflag);
	// for (int i = 0; i < 20 ; i++)
	// {
	// 	printf("i = %d == %c \n",i, def_term.c_cc[i]);
	// }
	// printf("==========================\n");

	printf("DEFAULT c_lflag\n");
	ten_to_two(def_term.c_cflag);
	printf("\n");


	// printf("~(ECHO | ICANON) \n");
	// ten_to_two(~(ECHO | ICANON));
	
	printf("ICANON\n");
	ten_to_two(ICANON);
	printf("\nECHO \n");
	ten_to_two(ECHO);

	def_term.c_cflag &= ~(ECHO | ICANON);
	printf("\n");

	printf("after\n");
	ten_to_two(def_term.c_cflag);
	printf("\n");

	
	
}