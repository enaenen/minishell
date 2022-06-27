/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 15:13:14 by wchae             #+#    #+#             */
/*   Updated: 2022/06/28 00:04:44 by wchae            ###   ########.fr       */
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

void	ft_sig_handler(int status)
{
	pid_t	pid;

	pid = waitpid(-1, 0, WNOHANG);
	if (status == SIGINT)
	{
		if (pid == -1)
		{
			write(1, "\n", 1);
			ft_replace_line(1);
		}
		else
		{
			write(1, "\n", 1);
			g_status = 130;
		}
	}
	else
		ft_replace_line(131);
}
	/*
	 else if (status == SIGQUIT && pid != -1)
	 {
	 	write(1, "Quit: 3\n", 8);
	 	g_status = 131;
	 }
	*/