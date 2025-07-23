/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_external.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emrozmen <emrozmen@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 12:36:36 by mecavus           #+#    #+#             */
/*   Updated: 2025/07/23 18:09:05 by emrozmen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_script_file(char *path)
{
	int		fd;
	char	buffer[3];
	int		bytes_read;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return (0);
	bytes_read = read(fd, buffer, 2);
	close(fd);
	if (bytes_read < 2)
		return (1);
	if (buffer[0] == '#' && buffer[1] == '!')
		return (0);
	return (1);
}

static void	handle_child_process(char **args, char *cmd_path, char **env_array)
{
	char	*sh_args[3];

	execve_signal();
	if (execve(cmd_path, args, env_array) == -1)
	{
		if (is_script_file(cmd_path))
		{
			sh_args[0] = "/bin/sh";
			sh_args[1] = cmd_path;
			sh_args[2] = NULL;
			if (execve("/bin/sh", sh_args, env_array) == -1)
			{
				perror("minishell");
				clear_exit(NULL, 126, NULL);
			}
		}
		else
		{
			perror("minishell");
			cmd_path = NULL;
			clear_exit(NULL, 126, NULL);
		}
	}
}

void	execute_external(char **args, t_env *env_list)
{
	pid_t	pid;
	char	**env_array;
	char	*cmd_path;

	cmd_path = find_command_path(args[0], env_list);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd("\n", 2);
		exit_status(127, PUSH);
		return ;
	}
	env_array = env_list_to_array(env_list);
	pid = fork();
	if (pid == 0)
		handle_child_process(args, cmd_path, env_array);
	else if (pid > 0)
	{
		cmd_path = NULL;
		wait_and_handle_status(pid);
	}
}

static void	handle_execve_failure(char *cmd_path, char **env_array)
{
	char	*sh_args[3];

	if (is_script_file(cmd_path))
	{
		sh_args[0] = "/bin/sh";
		sh_args[1] = cmd_path;
		sh_args[2] = NULL;
		if (execve("/bin/sh", sh_args, env_array) == -1)
		{
			perror("minishell");
			clear_exit(NULL, 126, NULL);
		}
	}
	else
	{
		perror("minishell");
		clear_exit(NULL, 126, NULL);
	}
}

void	execute_external_piped(char **args, t_env *env_list)
{
	char	**env_array;
	char	*cmd_path;

	cmd_path = find_command_path(args[0], env_list);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd("\n", 2);
		clear_exit(NULL, 127, NULL);
	}
	env_array = env_list_to_array(env_list);
	execve_signal();
	if (execve(cmd_path, args, env_array) == -1)
		handle_execve_failure(cmd_path, env_array);
}
