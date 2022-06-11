/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 17:08:37 by wchae             #+#    #+#             */
/*   Updated: 2022/06/12 17:10:27 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <signal.h>
# include <dirent.h>
# include <curses.h>
# include "libft.h"
# define ERROR -1
int g_status;

typedef struct s_list
{
	char			*data;
	struct s_list	*next;
}					t_list;

typedef struct s_env
{
	char 			*key;
	char 			*value;
	int				print_check;
	struct s_env	*next;
}	t_env;

typedef struct s_set
{
	struct termios	org_term;
	struct termios	new_term;
	int				org_stdin;
	int				org_stdout;
}	t_set;

typedef struct s_proc
{
	t_env	env_list;
	t_list	*data;
	t_list	*cmd;
	t_list	*limiter;
	int		infile;
	int		outfile;
	int		pip_flag;
	int		status;
}	t_proc;

void	ft_sig_handler(int status);



#endif