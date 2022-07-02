/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 23:53:43 by wchae             #+#    #+#             */
/*   Updated: 2022/06/30 19:54:46 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_env	*env_set(char	**envp)
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
	set->new_term.c_lflag &= ~(ECHOCTL);
	//1byte씩 처리
	set->new_term.c_cc[VMIN] = 1;
	//시간설정 사용 X
	set->new_term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &set->new_term);
}

static int	ft_env_lstsize(t_env	*lst)
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

static char	**convert_env_lst_to_dp(t_env	*env)
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
	set->new_term.c_lflag &= ~(ECHOCTL);
	set->new_term.c_cc[VMIN] = 1;
	set->new_term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &set->new_term);
	//SIGQUIT -> SIG_GIN (무시)
	signal(SIGQUIT, SIG_IGN);
}
