/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_exec_do_pipe.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/06 00:36:12 by wchae             #+#    #+#             */
/*   Updated: 2022/07/06 14:36:23 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../minishell.h"

// static int	count_pipe(t_b_node *root);
// static void	do_pipe_child(t_info *info, t_b_node *root, t_pipe_args args);
// static int	do_pipe_final_cmd(t_info *info, t_b_node *root, t_pipe_args args);
// static int	final_cmd_child(t_info *info, t_b_node *root, t_pipe_args args);


//exit utils
void	error_exit_wait(int n_wait)
{
	while (n_wait-- > 0)
		waitpid(-1, NULL, 0);
	exit(EXIT_FAILURE);
}

int	cnt_pipe(char **tokens)
{
	int	i;
	int	n_pipe;
	
	i = 0;
	n_pipe = 0;
	while (tokens[i])
	{
		if (ft_strncmp(tokens[i], "|", -1) == 0)
			n_pipe++;
		i++;
	}
	return (n_pipe);
}

int	is_builtin(char *cmd)
{
	int			i;
	static char	*built_in_fuc[7]
		= {"echo", "cd", "pwd", "export", "unset", "env", "exit"};

	i = 0;
	if (cmd == NULL)
		return (FALSE);
	while (i < 7)
		if (ft_strncmp(cmd, built_in_fuc[i++], -1) == 0)
			return (TRUE);
	return (FALSE);
}

int	do_main_builtin(t_info *info, t_b_node *root)
{
	int		io_fd[2];

	info->status = 0;
	if (root->redir)
	{
		backup_std_fd(io_fd);
		info->status = apply_redir(info, root);
	}
	if (!info->status)
		info->status = do_builtin(info, info->cmd);
	free_strs(info->cmd);
	info->cmd = NULL;
	if (root->redir)
		restore_std_fd(io_fd);
	return (info->status);
}

static void	do_pipe_child(char *token, t_pipe_args args)
{
	if (args.prev_pipe != -1)
	{
		dup2(args.prev_pipe, STDIN_FILENO);
		close(args.prev_pipe);
	}
	close(args.pipe_io[0]);
	dup2(args.pipe_io[1], STDOUT_FILENO);
	close(args.pipe_io[1]);
	// if (root->type == BT_PAREN)
		// args.status = do_pipe_paren(info, root);
	// else
	// {
	if (is_builtin(token))
		args.status = do_main_builtin(info, root);
	else
		args.status = do_cmd_child(info, root);
	// }
	exit(args.status);
}

int	do_pipe(char **tokens)
{
	t_pipe_args	args;
	int			i;

	i = 0;
	args.prev_pipe = -1;
	args.n_pipe = cnt_pipe(tokens);
	while (i++ < args.n_pipe)
	{
		if (pipe(args.pipe_io))
			error_exit_wait(i - 1);
		args.pid = fork();
		if (args.pid == -1)
			error_exit_wait(i - 1);
		else if (args.pid == 0)
			do_pipe_child(tokens[i - 1], args);
		else
		{
			close(args.pipe_io[1]);
			if (args.prev_pipe != -1)
				close(args.prev_pipe);
			args.prev_pipe = args.pipe_io[0];
			// root = root->right;
		}
	}
	return (do_pipe_final_cmd(info, root, args));
}


static int	do_pipe_final_cmd(t_info *info, t_b_node *root, t_pipe_args args)
{
	int	i;

	args.pid = fork();
	if (args.pid == -1)
		error_exit_wait(args.n_pipe);
	else if (args.pid == 0)
	{
		info->status = final_cmd_child(info, root, args);
		exit(info->status);
	}
	close(args.prev_pipe);
	i = 0;
	while (i < args.n_pipe + 1)
	{
		if (waitpid(-1, &args.status, 0) == args.pid)
			info->status = args.status;
		i++;
	}
	return (return_exit_status(info->status));
}

static int	final_cmd_child(t_info *info, t_b_node *root, t_pipe_args args)
{
	info->plv++;
	dup2(args.prev_pipe, STDIN_FILENO);
	close(args.prev_pipe);
	if (is_paren(root))
		args.status = do_pipe_paren(info, root);
	else
	{
		info->cmd = make_cmd_strs(info, root->tokens);
		if (info->cmd[0] && is_builtin(info->cmd[0]))
			args.status = do_main_builtin(info, root);
		else
			args.status = do_cmd_child(info, root);
	}
	return (args.status);
}
