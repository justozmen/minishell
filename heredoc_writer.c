/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_writer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emrozmen <emrozmen@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 14:37:12 by emrozmen          #+#    #+#             */
/*   Updated: 2025/07/22 14:39:30 by emrozmen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc_line(int fd, char *line,
				t_env *env_list, int expand)
{
	char	*expanded_line;

	if (expand)
	{
		expanded_line = expand_heredoc_line(line, env_list);
		write(fd, expanded_line, ft_strlen(expanded_line));
	}
	else
		write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	return (0);
}

static int	handle_heredoc_input(int fd, char *processed_delimiter,
				t_env *env_list, int expand)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || g_heredoc_interrupted)
		{
			if (g_heredoc_interrupted)
				_exit(130);
			break ;
		}
		if (ft_strcmp(line, processed_delimiter) == 0)
		{
			free(line);
			break ;
		}
		process_heredoc_line(fd, line, env_list, expand);
		free(line);
	}
	return (0);
}

static void	write_heredoc_child(int fd, char *processed_delimiter,
				t_env *env_list, int expand)
{
	int	result;

	init_heredoc_signal();
	result = handle_heredoc_input(fd, processed_delimiter, env_list, expand);
	close(fd);
	if (result == -1)
		_exit(130);
	else
		_exit(0);
}

static int	write_heredoc_parent(pid_t pid, int fd, char *filename)
{
	int	status;

	signal(SIGINT, SIG_IGN);
	close(fd);
	waitpid(pid, &status, 0);
	init_signal();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		unlink(filename);
		g_heredoc_interrupted = 1;
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		unlink(filename);
		g_heredoc_interrupted = 1;
		return (-1);
	}
	return (0);
}

int	handle_heredoc(t_token *current, t_env *env_list)
{
	char	*filename;
	char	*delimiter;
	int		expand;
	int		fd;

	if (!current || !current->next || current->type != HERDOC)
		return (-1);
	delimiter = current->next->value;
	expand = should_expand_heredoc(delimiter);
	filename = create_temp_filename();
	if (write_heredoc_to_file(filename, delimiter, env_list, expand) == -1)
		return (-1);
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		unlink(filename);
		return (-1);
	}
	current->fd_rdir = fd;
	current->value = filename;
	return (fd);
}
