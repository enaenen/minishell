/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 15:13:14 by wchae             #+#    #+#             */
/*   Updated: 2022/07/06 00:37:30 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_replace_line(int var_stat)
{
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_status = var_stat;
}

void	sig_readline(int sig)
{

	if (sig == SIGINT)
	{
		g_status = 1;
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	sig_here_doc(int sig)
{
	if (sig == SIGINT)
		printf("\n");
}
// void	sig_here_doc_child(int sig)
// {
// 	if (sig == SIGQUIT)
// 	{
// 		rl_replace_line("", 0);
// 		rl_on_new_line();
// 		rl_redisplay();
// 	}
// }

void	sig_exec(int sig)
{
	if (sig == SIGINT)
		printf("\n");
	else if (sig == SIGQUIT)
		printf("Quit: 3\n");
}