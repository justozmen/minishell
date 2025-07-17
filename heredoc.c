/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emrozmen <emrozmen@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 12:27:13 by emrozmen          #+#    #+#             */
/*   Updated: 2025/07/17 13:05:33 by emrozmen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	heredoc_signal_handler(int sig)
{
	(void)sig;
	ft_putstr_fd("\n", 1);
	exit(130);
}

static int is_quoted(const char *str)
{
	size_t len = ft_strlen((char *)str);
	return ((str[0] == '"' && str[len - 1] == '"') ||
			(str[0] == '\'' && str[len - 1] == '\''));
}

static char *heredoc_expand_line(char *line, __attribute__((unused)) t_env *env, int quoted)
{
	if (!quoted)
		return line; // Burada istersen variable expansion fonksiyonunu çağırabilirsin
	return line;
}

int	read_heredoc(const char *delimiter)
{
	char	*line;
	int		pipefd[2];
	int		quoted;

	if (pipe(pipefd) == -1)
	{
		perror("minishell: pipe");
		return (-1);
	}
	quoted = is_quoted(delimiter);
	signal(SIGINT, heredoc_signal_handler);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: heredoc delimited by end-of-file\n", 2);
			break ;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		line = heredoc_expand_line(line, NULL, quoted); // env parametresi ile expansion ekleyebilirsin
		ft_putstr_fd(line, pipefd[1]);
		ft_putstr_fd("\n", pipefd[1]);
		free(line);
	}
	close(pipefd[1]);
	signal(SIGINT, SIG_DFL);
	return (pipefd[0]);
}
