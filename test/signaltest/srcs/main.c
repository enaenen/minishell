/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 16:28:59 by wchae             #+#    #+#             */
/*   Updated: 2022/06/24 23:27:33 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**==============LinkedList============**/

void	ft_putstr(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		write(1, &str[i], 1);
		i++;
	}
}

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

t_list	*ft_lstlast(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	if (!lst || !new)
		return ;
	if (!*lst)
		*lst = new;
	else
		ft_lstlast(*lst)->next = new;
}

void	env_lstadd_back(t_env **lst, t_env *new, char *key, char *value)
{
	t_env	**phead;

	new = (t_env *)malloc(sizeof(t_env));
	new->key = key;
	new->value = value;
	new->print_check = 0;
	phead = lst;
	while (*phead)
		phead = &(*phead)->next;
	new->next = *phead;
	*phead = new;
}

t_list	*ft_lstnew(void *data)
{
	t_list	*new;

	new = (t_list *)malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->data = data;
	new->next = NULL;
	return (new);
}

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*next;

	if (!lst || !*lst || !del)
		return ;
	while (*lst)
	{
		next = (*lst)->next;
		del((*lst)->data);
		(*lst)->data = NULL;
		free(*lst);
		*lst = NULL;
		*lst = next;
	}
	*lst = NULL;
}


/*========== LinkedList END===========*/

t_env	*env_set(char	**envp)
{
	char	**splits;
	t_env	*tmp;
	t_env	*env_list;

	tmp = NULL;
	env_list = 0;
	while (*envp)
	{
	//env = 기준으로 split 후 list에 set
		splits = ft_split(*(envp++), '=');
		// printf("0 : %s\n",splits[0]);// printf("1 : %s\n",splits[1]);
		env_lstadd_back(&env_list, tmp, splits[0], splits[1]);
		free(splits);
	}
	return (env_list);
}

void	init_set(t_set *set, t_env **env, char **envp)
{
	//env "=" 기준으로 split
	*env = env_set(envp);
	g_status = 0;
	ft_memset(set, 0, sizeof(t_set));
	set->org_stdin = dup(STDIN_FILENO);
	set->org_stdout = dup(STDOUT_FILENO);
	tcgetattr(STDIN_FILENO, &set->org_term);
	tcgetattr(STDIN_FILENO, &set->new_term);
	set->new_term.c_lflag &= ECHO;
	//1byte씩 처리
	set->new_term.c_cc[VMIN] = 1;
	//시간설정 사용 X
	set->new_term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &set->new_term);
}

int		ft_env_lstsize(t_env	*lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

char	**convert_env_lst_to_dp(t_env	*env)
{
	int		i;
	int		len;
	char	**envp;
	char	*join_env;
	char	*tmp;

	len = ft_env_lstsize(env);
	envp = (char **)malloc(sizeof(char *) * len + 1);
	envp[len] = NULL;
	i = 0;
	while (len--)
	{
		tmp = ft_strjoin(env->key, "=");
		join_env = ft_strjoin(tmp, env->value);
		free(tmp);
		envp[i++] = join_env;
		env = env->next;
	}
	return (envp);
}

void	init_set2(t_set	*set, char ***envp, t_env *env)
{
	*envp = convert_env_lst_to_dp(env);
	tcgetattr(STDIN_FILENO, &set->new_term);
	set->new_term.c_lflag &= ECHO;
	set->new_term.c_cc[VMIN] = 1;
	set->new_term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &set->new_term);
	//SIGQUIT -> SIG_GIN (무시)
	signal(SIGQUIT, SIG_IGN);
}

void	reset_set(t_set *set)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &set->org_term);
}

/* UTIL */
int	error_msg(char *msg)
{
	g_status = 2;
	write(2, "bash: ", 6);
	if (!msg)
		write(2, "syntax error near unexpected token 'newline'", 44);
	else if (msg[0] == '|' || msg[0] == '<' || msg[0] == '>')
	{
		write(2, "syntax error near unexpected token '", 36);
		write(2, msg, ft_strlen(msg));
		write(2, "'\n", 2);
		return (ERROR);
	}
	else
	{
		g_status = 127;
		write(2, msg, ft_strlen(msg));
		if (ft_strncmp(strerror(errno), "Bad address", 12) == 0)
			write(2, ": command not found", 19);
		else
		{
			write(2, ": ", 2);
			write(2, strerror(errno), ft_strlen(strerror(errno)));
		}
	}
	write(2, "\n", 1);
	return (g_status);
}
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
			return (error_msg("MALLOC"));
		ft_lstadd_back(token, ft_lstnew(tmp));
		input = &input[i];
		i = 0;
	}
	while (input[i] == '<' || input[i] == '>')
		i++;
	tmp = ft_strntrim(input, " ", i);
	if (!tmp)
		return (error_msg("MALLOC"));
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
			return (error_msg("MALLOC"));
		ft_lstadd_back(token, ft_lstnew(tmp));
	}
	while (input[i] == ' ')
		i++;
	return (i);
}

int		split_pipe_token(char *input, int i, t_list **token)
{
	char	*tmp;

	if (i != 0)
	{
		tmp = ft_strntrim(input, " ", i);
		if (!tmp)
			return (error_msg("MALLOC"));
		ft_lstadd_back(token, ft_lstnew(tmp));
	}
	tmp = ft_strdup("|");
	if (!tmp)
		return (error_msg("MALLOC"));
		
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
/* HEREDOC */


char	*ft_merge_str(char *line, char buf)
{
	int		size;
	char	*str;
	int		i;

	size = ft_strlen(line);
	str = (char *)malloc(sizeof(char) * (size + 2));
	if (!str)
		return (NULL);
	i = 0;
	while (line[i] != 0)
	{
		str[i] = line[i];
		i++;
	}
	free(line);
	str[i++] = buf;
	str[i] = '\0';
	return (str);
}

int	get_next_line(char **line)
{
	char	buf;
	int		ret;

	*line = (char *)malloc(1);
	if (*line == NULL)
		return (-1);
	(*line)[0] = 0;
	ret = read(0, &buf, 1);
	while (buf != '\n' && buf != '\0')
	{
		*line = ft_merge_str(*line, buf);
		if (*line == 0)
			return (-1);
		ret = read(0, &buf, 1);
	}
	if (buf == '\n')
		return (1);
	return (0);
}

void	print_line(char *line, char *limiter, int fd)
{
	if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0)
		exit(0);
	write(1, "> ", 2);
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

int		ft_heredoc(char *limiter)
{
	char	*line;
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1)
		return (error_msg("pipe"));
	pid = fork();
	if (pid == 0)
	{
		close(fd[0]);
		write(1, "> ", 2);
		while (get_next_line(&line))
			print_line(line, limiter, fd[1]);
		close(fd[1]);
	}
	else if (0 < pid)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		waitpid(pid, 0, 0);
		close(fd[0]);
	}
	else
		return (error_msg("fork"));
	return (TRUE);
}

void	process_heredoc(t_list *token)
{
	int	org_stdin;

	org_stdin = dup(STDIN_FILENO);
	while (token)
	{
		if (ft_strncmp(token->data, "<<", 3) == 0)
		{
			dup2(org_stdin, STDIN_FILENO);
			ft_heredoc(token->next->data);
			token = token->next;
		}
		token = token->next;
	}
}

/* END HEREDOC */

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



void	parse_input(char *input, t_env *env, char **envp)
{
	t_list	*token;

	token = 0;
	add_history(input);
	if (split_token(input, &token) == TRUE && check_token(token) == TRUE)
	{
		process_heredoc(token);
		while (0 < waitpid(-1, &g_status, 0))
			continue ;
		ft_lstprint(token);
	}
	if (WIFEXITED(g_status))
		g_status = WEXITSTATUS(g_status);
	ft_lstclear(&token, free);
	write(1, &envp[0][0], 0);
	write(1, &env->value, 0);

}
/**
 * SETTING
 */
void	reset_stdio(t_set *set)
{
	// printf("org_stdin = %d\n", set->org_stdin);
	// printf("org_stdout = %d\n", set->org_stdout);
	dup2(set->org_stdin, STDIN_FILENO);
	dup2(set->org_stdout, STDOUT_FILENO);
}

void	ft_free_split(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

int main(int argc, char **argv, char **envp)
{
	t_set	set;
	t_env	*env;
	char	*input;

	argc = 0;
	argv = NULL;
	init_set(&set, &env, envp);
	signal(SIGINT, ft_sig_handler);
	while (1)
	{
		init_set2(&set, &envp, env);
		input = readline("minishell$ ");
		/*
		CTRL + D 처리 = NULL
		*/
		signal(SIGQUIT, ft_sig_handler);
		if (!input)
		{
			write(1,"exit\n", 5);
			reset_set(&set);
			exit(0);
		}
		tcsetattr(STDIN_FILENO, TCSANOW, &set.org_term);
		parse_input(input, env, envp);
		input = ft_free(input);
		//stdin , stdout 복구
		reset_stdio(&set);
		ft_free_split(envp);
	}
	return (0);
}