/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emrozmen <emrozmen@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 12:18:35 by mecavus           #+#    #+#             */
/*   Updated: 2025/07/23 18:08:49 by emrozmen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

void	handle_signal_status(int status)
{
	if (WTERMSIG(status) == SIGINT)
	{
		ft_putstr_fd("\n", 1);
		exit_status(130, PUSH);
	}
	else if (WTERMSIG(status) == SIGQUIT)
	{
		ft_putstr_fd("Quit (core dumped)\n", 1);
		exit_status(131, PUSH);
	}
}

void	wait_and_handle_status(pid_t pid)
{
	int	status;

	ignore_signal();
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_status(WEXITSTATUS(status), PUSH);
	else if (WIFSIGNALED(status))
		handle_signal_status(status);
	init_signal();
}

static void	free_paths(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = 0;
	while (paths[i])
		paths[i++] = NULL;
	paths = NULL;
}

static char	*try_path(char *path, char *command)
{
	char	*temp;
	char	*full_path;

	temp = ft_strjoin(path, "/");
	full_path = ft_strjoin(temp, command);
	temp = NULL;
	if (access(full_path, X_OK) == 0)
		return (full_path);
	full_path = NULL;
	return (NULL);
}

char	*find_command_path(char *command, t_env *env_list)
{
	char	**paths;
	char	*path_env;
	char	*full_path;
	int		i;

	if (ft_strchr(command, '/'))
		return (ft_strdup(command));
	path_env = get_env_value(env_list, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split_sep(path_env, ':');
	i = 0;
	while (paths[i])
	{
		full_path = try_path(paths[i], command);
		if (full_path)
		{
			free_paths(paths);
			return (full_path);
		}
		i++;
	}
	free_paths(paths);
	return (NULL);
}
