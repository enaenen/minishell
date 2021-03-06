/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 16:28:59 by wchae             #+#    #+#             */
/*   Updated: 2022/07/06 00:53:45 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/ttydefaults.h>
/**==============LinkedList============**/

void	ft_lstprint(t_list *lst)
{
	if (!lst)
		return ;
	while (lst)
	{
		ft_putstr(lst->data);
		ft_putstr("\n");
		lst = lst->next;
	}
		
}
/*========== LinkedList END===========*/


// void	reset_set(t_set *set)
// {
// 	tcsetattr(STDIN_FILENO, TCSANOW, &set->org_term);
// }

/* UTIL */

/* UTIL END*/

/* SPLIT TOKEN */
/** QUOTE**/
int		 find_valid_quot_point(char *data, int start)
{
	int find;

	find = start + 1;
	//닫는게 있다면 찾은지점 index return
	while (data[find] && data[start] != data[find])
		find++;
	if (data[find])
		return (find);
	//없다면 시작지점 return
	return (start);
}

/**
 *
 * 쪼개는 작업
 *  "echo abc > a.txt"
 * 	"echo abc >" AND "a.txt"
 **/

int		split_redirection_token(char *input, int i, t_list **token)
{
	char	*tmp;
	int		save;

	save = i;
	if (i != 0)
	{
		tmp = ft_strntrim(input, " ", i);
		if (!tmp)
			return (error_msg("malloc"));
		ft_lstadd_back(token, ft_lstnew(tmp));
		input = &input[i];
		i = 0;
	}
	while (input[i] == '<' || input[i] == '>')
		i++;
	tmp = ft_strntrim(input, " ", i);
	if (!tmp)
		return (error_msg("malloc"));
	ft_lstadd_back(token, ft_lstnew(tmp));
	return (i + save);
}

int	split_space_token(char *input, int i, t_list **token)
{
	char	*tmp;

	if (i != 0)
	{
		tmp = ft_strntrim(input, " ", i);
		if (!tmp)
			return (error_msg("malloc"));
		ft_lstadd_back(token, ft_lstnew(tmp));
	}
	while (input[i] == ' ')
		i++;
	return (i);
}
/**
 *
 * 쪼개는 작업
 *  "echo abc | ls -l"
 * 	"echo abc" AND "|"" AND "ls -l"
 **/

int		split_pipe_token(char *input, int i, t_list **token)
{
	char	*tmp;

	if (i != 0)
	{
		tmp = ft_strntrim(input, " ", i);
		if (!tmp)
			return (error_msg("malloc"));
		ft_lstadd_back(token, ft_lstnew(tmp));
	}
	tmp = ft_strdup("|");
	if (!tmp)
		return (error_msg("malloc"));
		
	ft_lstadd_back(token, ft_lstnew(tmp));
	return (i + 1);
}

int		split_rest_token(char *input, t_list **token)
{
	char	*tmp;
	
	if (input[0])
	{
		tmp = ft_strntrim(input, " ", ft_strlen(input));
		if (!tmp)
			return (error_msg("MALLOC"));
		ft_lstadd_back(token, ft_lstnew(tmp));
	}
	return (TRUE);
}

int		split_token(char *input, t_list **token)
{
	int	i;

	i = -1;
	while (input[++i])
	{
		if (input[i] == '\"' || input[i] == '\'')
		{
			i = find_valid_quot_point(input, i);
			continue ;
		}
		else if (input[i] == '<' || input[i] == '>')
			i = split_redirection_token(input, i, token);
		else if (input[i] == ' ')
			i = split_space_token(input, i , token);
		else if (input[i] == '|')
			i = split_pipe_token(input, i, token);
		else
			continue ;
		if (i == ERROR)
			return (ERROR);
		input = &input[i];
		i = -1;
	}
	return (split_rest_token(input, token));
}

/* SPLIT TOKEN END */

/* CHECK TOKEN */

int		check_token(t_list	*token)
{
	int	i;

	i = 0;
	while (token)
	{
		// | 나 ||밖에없을때
		if (token->data[0] == '|' && (i == 0 
			|| !token->next || token->next->data[0] == '|'))
			return (error_msg("|"));
		// < | > 관련 에러처리
		else if (token->data[0] == '<' || token->data[0] == '>')
		{	
			if (token->data[1] && token->data[0] != token->data[1])
				return (error_msg(&token->data[1]));
			else if (2 < ft_strlen(token->data))
				return (error_msg(&token->data[2]));
			else if (!token->next)
				return (error_msg(NULL));
			else if (token->next->data[0] == '<'
				|| token->next->data[0] == '>'
				|| token->next->data[0] == '|')
				return (error_msg(token->next->data));
		}
		i++;
		token = token->next;
	}
	return (TRUE);
}

/* PIPE TOKEN */

/** IN PIPE _ TOKEN PROCESS **/
char	*ms_strtrim(char *data, int start, int end)
{
	int		i;
	int		j;
	char	*ret;

	ret = (char *)malloc(end * sizeof(char));
	if (!ret)
		return (NULL);
	i = -1;
	j = 0;
	while (data[++i] && i < end)
	{
		if (i != start)
			ret[j++] = data[i];
	}
	ret[j] = '\0';
	return (ret);
}

// data를 받아 data +1 부터 $ 토큰을 찾음 
int		find_env_var_token(char *data, int start, int end)
{
	int	find;

	find = start + 1;
	while (data[find] && find < end && data[find] != '$')
		find++;
	if (find == end)
		return (FALSE);
	return (TRUE);
}
/** env 해석 
 * read_key env 해석 하여 key의 value 값 가져옴
 * strjoin 으로 합치기
 * **/

char	*read_key(t_env *env_list, char *key)
{
	while (env_list)
	{
		if (!ft_strcmp(env_list->key, key))
			return (ft_strdup(env_list->value));
		env_list = env_list->next;
	}
	return (NULL);
}


char	*parse_pre_env_var(char *data, int start, char *new_data)
{
	char	*org_data;
	char	*tmp;

	org_data = new_data;
	tmp = ft_strndup(data, start);
	if (!tmp)
		return (ft_free(org_data));
	new_data = ft_strjoin(new_data, tmp);
	ft_free(tmp);
	ft_free(org_data);
	return (new_data);
	
}


int		find_valid_env_var_point(char *data)
{
	int	index;
	if (data[0] == '?')
		return (1);
	index = 0;
	while (data[index] && (ft_isalnum(data[index]) || data[index] == '_'))
		index++;
	return (index);
}


char	*expand_env_var(t_proc *proc, char *data, int start, char **new_data)
{
	char	*tmp;
	char	*get_env;

	*new_data = parse_pre_env_var(data, start, *new_data);
	if (!(*new_data))
		return (NULL);
	data = &data[start + 1];
	//$ 이후에 num, alnum or _ 일때
	get_env = ft_strndup(data, find_valid_env_var_point(data));
	data = &data[find_valid_env_var_point(data)];
	if (!get_env)
		return (ft_free(*new_data));
	tmp = get_env;
	if (get_env[0] == '?')
		get_env = ft_itoa(g_status);
	else
		get_env = read_key(proc->env_list, get_env);
	ft_free(tmp);
	tmp = *new_data;
	*new_data = ft_strjoin(*new_data, get_env);
	ft_free(tmp);
	ft_free(get_env);
	if (!(*new_data))
		return (NULL);
	return (data);
}

char	*expand_int_quot_utils(t_proc *proc, char *data, char **new_data)
{
	int i;
	i = -1;
	while (data[++i])
	{
		if (data[i]== '$')
		{
			data = expand_env_var(proc, data, i ,new_data);
			if (!data)
				return (ft_free(*new_data));
			i = -1;
		}
	}
	return (data);
}

char	*expand_in_quot_utils(t_proc *proc, char *data, char **new_data)
{
	int i;
	
	i = -1;
	while (data[++i])
	{
		if (data[i] == '$')
		{
			data = expand_env_var(proc, data, i, new_data);
			if (!data)
				return (ft_free(*new_data));
			i = -1;
		}
	}
	return (data);
}

char	*expand_in_quot_env_var(t_proc *proc, char *data, int start, int end)
{
	char	*new_data;
	char	*tmp;
	char	*tmp2;

	new_data = ft_strndup(data, start);
	if (!new_data)
		return (NULL);
	data = &data[start + 1];
	data = ft_strndup(data, end - start - 1);
	if (!data)
		return (ft_free(new_data));
	tmp2 = data;
	data = expand_in_quot_utils(proc, data, &new_data);
	if (!data)
		return (ft_free(tmp2));
	tmp = new_data;
	new_data = ft_strjoin(new_data, data);
	ft_free(tmp2);
	ft_free(tmp);
	return (new_data);
}
/*QUOT 처리*/
//data 는 quot 뒤엣부분
char	*del_big_quot(t_proc *proc, char *data, int start, char **new_data)
{
	int		end;
	char	*org_data;
	char	*tmp;

	org_data = *new_data;
	tmp = NULL;
	end = find_valid_quot_point(data, start);
	if (find_env_var_token(data, start, end) == TRUE)
		tmp = expand_in_quot_env_var(proc, data, start, end);
	else
		tmp = ms_strtrim(data, start, end);
	if (!tmp)
		return (ft_free(org_data));
	*new_data = ft_strjoin(*new_data, tmp);
	ft_free(org_data);
	ft_free(tmp);
	if (!(*new_data))
		return (NULL);
	data = &data[end + 1];
	return (data);
}

char	*del_small_quot_token(char *data, int start, char **new_data)
{
	int		end;
	char	*tmp;
	char	*org_data;
	
	org_data = *new_data;
	end = find_valid_quot_point(data, start);
	tmp = ms_strtrim(data, start, end);
	if (!tmp)
		return (NULL);
	*new_data = ft_strjoin(*new_data, tmp);
	ft_free(org_data);
	ft_free(tmp);
	if (!(*new_data))
		return (NULL);
	data = &data[end + 1];
	return (data);
}
/** END IN PIPE _ TOKEN PROCESS **/
/** expand_var**/

char	*expand_in_quot_uitls(t_proc *proc, char *data, char **new)
{
	int	i;

	i = -1;
	while (data[++i])
	{
		if (data[i] == '$')
		{
			data = expand_env_var(proc, data, i , new);
			if (!data)
				return(ft_free(*new));
			i = -1;
		}
	}
	return (data);
}
//malloc error
char	*expand_data(t_proc *proc, char *data)
{
	char	*new_data;
	char	*tmp;
	int		i;

	i = -1;
	new_data = NULL;
	while (data[++i])
	{
		// ' " 제거
		if (data[i] == '\'' && i != find_valid_quot_point(data, i))
			data = del_small_quot_token(data, i, &new_data);
		else if (data[i] == '\"' && i != find_valid_quot_point(data, i))
			data = del_big_quot(proc, data, i, &new_data);
		else if (data[i] == '$')
			data = expand_env_var(proc, data, i, &new_data);
		else
			continue;
		if (!data)
			return (ft_free(new_data));
		i = -1;
	}
	tmp = new_data;
	new_data = ft_strjoin(new_data, data);
	ft_free(tmp);

	return (new_data);
}

// >>  << < > 일때 리다이렉션 처리
int		parse_std_inout_redirection(t_proc *proc, t_list *data, char *tmp)
{
	if (ft_strncmp(data->data, "<<", 3) == 0)
		ft_lstadd_back(&proc->limiter, ft_lstnew(ft_strdup(tmp)));
	if (ft_strncmp(data->data, "<", 3) == 0)
	{
		proc->infile = open(tmp, O_RDONLY);
		if (proc->infile < 0)
		{
			error_msg(tmp);
			ft_free(tmp);
			return (ERROR);
		}
		dup2(proc->infile, STDIN_FILENO);
	}
	if (ft_strncmp(data->data, ">>", 3) == 0)
		proc->outfile = open(tmp, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (ft_strncmp(data->data, ">", 2) == 0)
		proc->outfile = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (proc->outfile < 0)
	{
			error_msg(tmp);
			ft_free(tmp);
			return (ERROR);
	}
	return (TRUE);
}

int		parse_data(t_proc *proc, t_list *data)
{
	char	*tmp;
	while (data)
	{
		if (data->data[0] == '<' || data->data[0] == '>')
		{
			tmp = expand_data(proc, data->next->data);
			if (!tmp)
				return (error_msg("malloc"));
			else if (parse_std_inout_redirection(proc, data, tmp) == ERROR)
				return (ERROR);
			ft_free(tmp);
			data = data->next;
		}
		else
		{
			tmp = expand_data(proc, data->data);
			if (!tmp)
				return (error_msg("malloc"));
			ft_lstadd_back(&proc->cmd, ft_lstnew(tmp));
		}
		data = data->next;
	}
	return (TRUE);
}

char	**split_cmd(t_list *cmd)
{
	char	**exe;
	int		size;
	int		i;

	size = ft_lstsize(cmd);
	exe = (char **)malloc((size + 1) * sizeof(char *));
	if (!exe)
		return (NULL);
	i = 0;
	while (i < size)
	{
		exe[i] = cmd->data;
		cmd = cmd->next;
		i++;
	}
	exe[i] = NULL;
	return (exe);
}

char	*find_path(char *cmd, char **env_list, int i)
{
	char	*path;
	char	**paths;
	char	*tmp;
	while (env_list[i] && ft_strnstr(env_list[i], "PATH=", 5) == NULL)
	{
		// printf("envlist = %s\n",env_list[i]);
		i++;
	}
	if (env_list[i] == NULL)
		return (cmd);
	paths = ft_split(env_list[i] + 5, ':');
	i = 0;
	while (paths[i])
	{
		path = ft_strjoin(paths[i], "/");
		tmp = path;
		path = ft_strjoin(path, cmd);
		free(tmp);
		if (access(path, F_OK) == 0)
		{
			free(paths);
			return (path);
		}
		i++;
	}
	return (cmd);
}

/**COMMAND HANDLING**/
int		execute_cmd(t_proc *proc, t_list *cmd, int *fd, char **envp)
{
	char **exe;

	close(fd[0]);
	if (0 < proc->outfile)
		dup2(proc->outfile, STDOUT_FILENO);
	else
		dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	exe = split_cmd(cmd);
	if (!exe)
		return (error_msg("malloc"));
	if (check_builtin_cmd(proc->cmd) == TRUE)
		execute_builtin_cmd(proc, exe);
	else if (exe[0][0] == '/' || exe[0][0] == '.')
	{
		if (execve(exe[0], exe, 0) == -1)
			return (error_msg(exe[0]));
	}
	else if (execve(find_path(exe[0], envp, 0), exe, envp) == -1)
		return (error_msg(exe[0]));
	return (0);
}

int		handle_cmd(t_proc *proc, t_list *cmd, char **envp)
{
	int		fd[2];
	pid_t	pid;
	// printf("proc->cmd\n");
	ft_lstprint(proc->cmd);
	// printf("proc->data\n");
	ft_lstprint(proc->data);
	// printf("proc->cmd\n");
	// ft_lstprint(proc->cmd);

	if (pipe(fd) == -1)
		return (error_msg("pipe"));
	pid = fork();
	if (pid == 0)
		exit(execute_cmd(proc, cmd, fd, envp));
	else if (0 < pid)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
	}
	else
	{
		close(fd[0]);
		close(fd[1]);
		return (error_msg("fork"));
	}
	return (0);
}

/**COMMAND HANDLING END **/


int		parse_process(t_proc *proc, t_env *env, char **envp)
{
	proc->env_list = env;
	if (parse_data(proc, proc->data) == TRUE && proc->cmd)
		handle_cmd(proc, proc->cmd, envp);
	ft_lstclear(&proc->limiter, free);
	ft_lstclear(&proc->cmd, free);
	ft_lstclear(&proc->data, free);
	return (TRUE);
}

/** pasre process **/
/*
static int	make_path_and_exec_cmd(t_proc *info, char **path)
{
	t_buffer	*buf;
	char		**env;
	int			i;

	if (!path)
		return (0);
	env = get_env_strs(info);
	buf = create_buf();
	i = 0;
	while (path[i])
	{
		add_str(buf, path[i]);
		add_char(buf, '/');
		add_str(buf, proc->cmd[0]);
		free(path[i]);
		path[i] = put_str(buf);
		execve(path[i], info->cmd, env);
		if (errno != ENOENT)
			break ;
		i++;
	}
	del_buf(buf);
	free_strs(env);
	return (i);
}


static int	do_cmd_env_path(t_proc *proc, t_env *path_node)
{
	char		**path;
	int			i;

	path = ft_split(path_node->value, ':');
	i = make_path_and_exec_cmd(proc, path);
	if (errno == ENOENT)
		print_err_msg(info->cmd[0], "ECMDNF");
	else if (path)
	{
		if (ft_strncmp(info->cmd[0], "..", -1) == 0)
			print_err_msg(info->cmd[0], EDIR);
		else
			print_err_msg(path[i], strerror(errno));
	}
	else
		print_err_msg(info->cmd[0], strerror(ENOENT));
	free_strs(info->cmd);
	free_strs(path);
	return (ERROR_EXIT);
}
*/

int other_command(t_proc *proc, t_list *cmd)
{
	pid_t	pid;
	// t_env	*path_node;

	char	**exe;
	char	**new_envp;

	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
	if (pid == 0)
	{

		// path_node = find_env_node(proc->env_list, "PATH");
		// if (path_node)
		// 	return (do_cmd_env_path(proc, path_node));


		new_envp = get_env_list(&proc->env_list);
		if (0 < proc->outfile)
			dup2(proc->outfile, STDOUT_FILENO);
		exe = split_cmd(cmd);
		if (!exe)
			return (error_msg("malloc"));
		if (check_builtin_cmd(proc->cmd) == TRUE)
			execute_builtin_cmd(proc, exe);
		else if (exe[0][0] == '/' || exe[0][0] == '.')
		{
			proc->status = execve(exe[0], exe, new_envp);
		}
		else
		{
			proc->status = execve(find_path(exe[0], new_envp, 0), exe, new_envp);
		}
		if (proc->status == -1)
			exit(error_msg(exe[0]));
	}

	else if (0 < pid)
		return (0);
	else
		return (error_msg("fork"));
	return (0);
}

int parse_last_process(t_proc *proc, t_env *env)
{
	char	**exe;

	proc->env_list = env;
	exe = NULL;
	// write(1, *envp, 0);
	//data expand
	if (parse_data(proc, proc->data) == TRUE && proc->cmd)
	{
		signal(SIGINT, &sig_exec);
		signal(SIGQUIT, &sig_exec);
		// write(1,"proc->data :", ft_strlen("proc->data :"));
		// ft_lstprint(proc->data);
		// ft_lstprint(proc->cmd);
		if (check_builtin_cmd(proc->cmd))
		{
			if (0 < proc->outfile)
				dup2(proc->outfile, STDOUT_FILENO);
			exe = split_cmd(proc->cmd);
			if (!exe)
				return (error_msg("malloc"));
			execute_builtin_cmd(proc, exe);
		}
		else
			other_command(proc, proc->cmd);
	}
	ft_lstclear(&proc->limiter, free);
	ft_lstclear(&proc->cmd, free);
	ft_lstclear(&proc->data, free);
	return (TRUE);
}
/** parse process END **/
char	**token_to_str(t_list *token)
{
	char		**strs;
	t_list		*tmp;
	int			i;

	i = 0;
	tmp = token;
	while (tmp)
	{
		tmp = tmp->next;
		i++;
	}
	strs = malloc((i + 1) * sizeof(char *));
	if (strs == NULL)
		return (NULL);
	strs[i] = NULL;
	i = 0;
	tmp = token;
	while (tmp)
	{
		strs[i++] = ft_strdup(tmp->data);
		tmp = tmp->next;
	}
	return (strs);
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

int		parse_pipe_token(t_list *token, t_env *env)
{
	char	*tmp;
	t_proc	proc;
	char	**tokens;
	int		n_pipe;

	ft_memset(&proc, 0, sizeof(t_proc));
	tokens = token_to_str(token);
	n_pipe = cnt_pipe(tokens);
	proc.env_list = env;
	// parse_data(&proc, proc.data);
	if (0 < n_pipe)
	{
		printf("pipe process\n");
	}
	else
	{
		while (token)
		{
			if (token->data[0] != '|')
			tmp = ft_strdup(token->data);
			if (!tmp)
				return (error_msg("malloc"));
			ft_lstadd_back(&proc.data, ft_lstnew(tmp));
			// ft_lstprint(proc.data);
			if (!token->next)
				parse_last_process(&proc, env);
			token = token->next;
		}
	}
	/*
	while (token)
	{
		if (token->data[0] != '|')
		{
			tmp = ft_strdup(token->data);
			if (!tmp)
				return (error_msg("malloc"));
			ft_lstadd_back(&proc.data, ft_lstnew(tmp));
			// ft_lstprint(proc.data);
		}
		if (token->data[0] == '|')
		{
			ft_memset(&proc, 0, sizeof(t_proc));
			proc.pipe_flag += TRUE;
			parse_process(&proc, env, envp);
		}
		
		if (!token->next)
			parse_last_process(&proc, env, envp);
		token = token->next;
	}
	*/
	ft_free_split(tokens);
	return (TRUE);
}
/* END PIPE*/


/* CHECK TOKEN END */

void	parse_input(char *input, t_env *env)
{
	t_list	*token;
	int		pipe_cnt;

	token = 0;
	pipe_cnt = 0;
	add_history(input);
	if (split_token(input, &token) == TRUE && check_token(token) == TRUE)
	{
		process_heredoc(token);
		parse_pipe_token(token, env);
		while (0 < waitpid(-1, &g_status, 0))
			continue ;
		// ft_lstprint(token);
	}
	if (WIFEXITED(g_status))
		g_status = WEXITSTATUS(g_status);
	ft_lstclear(&token, free);
}
/**
 * SETTING
 */
void	reset_stdio(t_set *set)
{
	dup2(set->org_stdin, STDIN_FILENO);
	dup2(set->org_stdout, STDOUT_FILENO);
}
/**fortest**/
void	print_env_list(t_env *env)
{
	while (env){
		printf("key = %s value = %s \n",env->key, env->value);
		env = env->next;
	}
}

int main(void)
{
	t_set	set;
	t_env	*env;
	char	*input;
	char	**envp;

	init_set(&set, &env);
	envp = get_env_list(&env);

	while (1)
	{
		signal(SIGINT, &sig_readline);
		signal(SIGQUIT, SIG_IGN);
		tcsetattr(STDOUT_FILENO, TCSANOW, &set.new_term);
		input = readline("minishell$ ");
		if (input == NULL)
		{
			write(1,"\e[Aminishell$ exit\n", 20);
			tcsetattr(STDOUT_FILENO, TCSANOW, &set.org_term);
			exit(g_status);
		}
		parse_input(input, env);
		input = ft_free(input);
		reset_stdio(&set);
		// ft_print_envlist(get_env_list(&env));
	}
	// signal(SIGINT, &sig_readline);
	// while (1)
	// {
		// init_set2(&set, &envp, env);
		// /*
		// CTRL + D 처리 = NULL
		// */
		// signal(SIGQUIT, &sig_readline);
		// if (!input)
		// {
		// 	write(1,"exit\n", 5);
		// 	tcsetattr(STDIN_FILENO, TCSANOW, &set.org_term);
		// 	exit(0);
		// }
		// tcsetattr(STDIN_FILENO, TCSANOW, &set.org_term);
		// parse_input(input, env, envp);
		// input = ft_free(input);
		// //stdin , stdout 복구
		// reset_stdio(&set);
		// ft_free_split(envp);
	// }
	return (0);
}