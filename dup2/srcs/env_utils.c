/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchae <wchae@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 02:25:03 by wchae             #+#    #+#             */
/*   Updated: 2022/07/04 04:44:00 by wchae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// static void get_env_convert(t_env **env_list, char **env)
// {
// 	int		i;
// 	t_env	*tmp;
// 	char	*tmp2;
	
// 	i = 0;
// 	tmp = *env_list;
// 	while (tmp)
// 	{
// 		if (tmp->value)
// 		{
// 			tmp2 = ft_strjoin(env[i], tmp->key);
// 			ft_free(env[i]);
// 			env[i] = tmp2;

// 			tmp2 = ft_strjoin(env[i], "=");
// 			ft_free(env[i]);
// 			env[i] = tmp2;

// 			tmp2 = ft_strjoin(env[i], tmp->value);
// 			ft_free(env[i]);
// 			env[i] = tmp2;
// 			i++;
// 		}
// 		tmp = tmp->next;
// 	}
// }

static void	get_env_convert(t_env **env_list, char **env)
{
	t_env		*tmp;
	t_buffer	*buf;
	int			i;

	buf = create_buf();
	tmp = *env_list;
	i = 0;
	while (tmp)
	{
		if (tmp->value)
		{
			add_str(buf, tmp->key);
			add_char(buf, '=');
			add_str(buf, tmp->value);
			env[i++] = put_str(buf);
		}
		tmp = tmp->next;
	}
	del_buf(buf);
}

char **get_env_list(t_env **env_list)
{
	t_env	*tmp;
	char	**env;
	int		i;

	i = 0;
	tmp = *env_list;
	while (tmp)
	{
		if (tmp->value)
			i++;
		tmp = tmp->next;
	}
	env = malloc(sizeof(char *) * (i + 1));
	if (env == NULL)
		exit(1);
	get_env_convert(env_list, env);
	return (env);
}

// t_env	*find_env_node(t_env *env_list, char *key)
// {
// 	while (env_list)
// 	{
// 		if (ft_strncmp(env_list->key, key, -1) == 0)
// 			return (env_list);
// 		env_list = env_list->next;
// 	}
// 	return (NULL);
// }
