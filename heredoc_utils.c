/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emrozmen <emrozmen@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:23:07 by mecavus           #+#    #+#             */
/*   Updated: 2025/07/22 14:39:35 by emrozmen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	write_heredoc_to_file(char *filename, char *delimiter,
			t_env *env_list, int expand)
{
	int		fd;
	char	*processed_delimiter;
	pid_t	pid;

	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
		return (-1);
	processed_delimiter = process_heredoc_delimiter(delimiter);
	pid = fork();
	if (pid == 0)
		write_heredoc_child(fd, processed_delimiter, env_list, expand);
	else if (pid > 0)
		return (write_heredoc_parent(pid, fd, filename));
	close(fd);
	return (-1);
}
typedef struct s_delimiter_state
{
	int	s_quot;
	int	d_quot;
	int	i;
}	t_delimiter_state;

static char	*handle_quote_in_delimiter(char *result, char *delimiter,
				t_delimiter_state *state)
{
	if (delimiter[state->i] == '\'')
	{
		if (state->d_quot)
			result = append_char(result, delimiter, &state->i);
		else
		{
			state->s_quot = !(state->s_quot);
			state->i++;
		}
	}
	else if (delimiter[state->i] == '\"')
	{
		if (state->s_quot)
			result = append_char(result, delimiter, &state->i);
		else
		{
			state->d_quot = !(state->d_quot);
			state->i++;
		}
	}
	else
		result = append_char(result, delimiter, &state->i);
	return (result);
}

char	*process_heredoc_delimiter(char *delimiter)
{
	char				*result;
	t_delimiter_state	state;

	if (!delimiter)
		return (NULL);
	if (ft_strlen(delimiter) == 0)
		return (ft_strdup(delimiter));
	result = ft_strdup("");
	state.i = 0;
	state.s_quot = 0;
	state.d_quot = 0;
	while (delimiter[state.i])
		result = handle_quote_in_delimiter(result, delimiter, &state);
	return (result);
}

int	should_expand_heredoc(char *original_delimiter)
{
	int	i;

	if (!original_delimiter)
		return (1);
	i = 0;
	while (original_delimiter[i])
	{
		if (original_delimiter[i] == '"' || original_delimiter[i] == '\'')
			return (0);
		i++;
	}
	return (1);
}

char	*create_temp_filename(void)
{
	static int	heredoc_count = 0;
	char		*num_str;
	char		*filename;
	char		*temp;

	num_str = ft_itoa(heredoc_count++);
	temp = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	filename = ft_strjoin(temp, ".tmp");
	return (filename);
}
