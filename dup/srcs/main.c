/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 16:28:59 by wchae             #+#    #+#             */
/*   Updated: 2022/06/18 02:42:21 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**==============LinkedList============**/

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
int		find_valid_quot_point(char *data, int start)
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
/* WIP
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
		return (error_msg("MALLOC PROBLEM"));
	ft_lstadd_back(token, ft_lstnew(tmp));
	return (i + save);
}
*/
int	split_space_token(char *input, int i, t_list **token)
{
	char	*tmp;

	if (i != 0)
	{
		tmp = ft_strntrim(input, " ", i);
		if (!tmp)
			return (error_msg("MALLOC PB"));
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
		/*
		WIP
		else if (input[i] == '<' || input[i] == '>')
			i = split_redirection_token(input, i, token);
		*/
		else if (input[i] == ' ')
			i = split_space_token(input, i , token);
		else if (input[i] == '|')
			i = split_pipe_token(input, i, token);
			//split pipe token
		else
			continue ;
		if (i == ERROR)
			return (ERROR);
		input = &input[i];
		i = -1;
	}
	return 0;
}

/* SPLIT TOKEN END */


void	parse_input(char *input, t_env *env, char **envp)
{
	t_list	*token;
	token = 0;
	add_history(input);
	if (split_token(input, &token) == TRUE && check_token(token) == TRUE)
	{

	}
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
		input = readline("$ ");
		/*
		CTRL + D 처리 =NULL
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
		// reset_stdio(&set);
		// ft_free_split(envp);
	}
	return (0);
}