#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <termios.h>

void	sig_handler(int signal)
{
	if (signal == SIGINT)
	{
		printf("nanoshell$ \n");
	}
	if (rl_on_new_line() == -1)	// write readline buffer
		exit(1);
	rl_replace_line("", 1);		// erase contents on Prompt
	rl_redisplay();			// prompt curser not moving
}

void	setting_signal()
{
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
}

int	main(int argc, char **argv, char **envp)
{
	char	*str;
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ECHOCTL);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	setting_signal();

	while (1)
	{
		str = readline("nanoshell$ ");
		if (!str)
		{
			printf("\033[1A");
			printf("\033[10C");
			printf(" exit\n");
			exit(-1);
		}
		else if (*str == '\0')
			free(str);
		else
		{
			add_history(str);
			printf("%s\n", str);
			free(str);
		}
	}
	return (0);
}
