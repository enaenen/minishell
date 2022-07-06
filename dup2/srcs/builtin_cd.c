/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/28 20:05:39 by wchae             #+#    #+#             */
/*   Updated: 2022/07/04 22:06:06 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char *read_key(t_env *env_list, char *key)
{
	while (env_list)
	{
		if (ft_strcmp(env_list->key, key) == 0)
			return (ft_strdup(env_list->value));
		env_list = env_list->next;
	}
	return (NULL);
}

static void	cd_error(int error, char *arg)
{
	char	*errmsg;

	errmsg = strerror(error);
	write(2, "bash: cd: ", 10);
	write(2, arg, ft_strlen(arg));
	write(2, ": ", 2);
	write(2, errmsg, ft_strlen(errmsg));
	write(2, "\n", 1);
}



void	ft_cd(char **buf, t_env *env_list)
{
	int		tmp;
	int		error;
	char	*pwd;
	char	*old_pwd;
	
	old_pwd = getcwd(NULL, 0);
	if (!buf[0])
	{
		chdir(read_key(env_list, "HOME"));
		g_status = 0;
		return ;
	}
	tmp = chdir(buf[0]);
	if (tmp < 0)
	{
		//TODO $PWD , $OLD_PWD 갱신
		error = errno;
		cd_error(error, buf[0]);
		g_status = error;
		return ;
	}
	
	pwd = getcwd(NULL, 0);
	if(pwd && old_pwd)
	{
		if (find_env_node(env_list, "PWD"))
			set_env_node(&env_list, ft_strdup("PWD"), pwd);
		if (find_env_node(env_list, "OLDPWD"))
			set_env_node(&env_list, ft_strdup("OLDPWD"), old_pwd);
	}

	g_status = 0;
}
