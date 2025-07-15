/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mecavus <mecavus@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 16:44:32 by emrozmen          #+#    #+#             */
/*   Updated: 2025/07/14 16:31:35 by mecavus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_quotes(const char *s)
{
	int	i;
	int	sq;
	int	dq;

	i = 0;
	sq = 0;
	dq = 0;
	while (s[i])
	{
		if (s[i] == '\'' && dq == 0)
			sq = !sq;
		else if (s[i] == '\"' && sq == 0)
			dq = !dq;
		i++;
	}
	return (sq || dq);
}

static int	skip_quote(char *s, int i)
{
	char	q;

	q = s[i];
	i++;
	while (s[i] && s[i] != q)
		i++;
	return (i);
}

static int	check_operator(char *s, int i)
{
	char	c;
	int		count;

	c = s[i];
	count = 0;
	while (s[i] == c)
	{
		count++;
		i++;
	}
	if ((c == '|' && count > 1) || count > 2)
		return (1);
	return (0);
}

static int	skip_op_and_space(char *s, int i)
{
	char	c;

	c = s[i];
	while (s[i] == c)
		i++;
	i = skip_spaces(s, i);
	return (i);
}

int	check_syntax(char *s)
{
	int	i;

	if (!s || check_quotes(s))
		return (ft_putstr_fd("bad quote\n", 2), exit_status(2, PUSH), 1);
	i = skip_spaces(s, 0);
	if (s[i] == '|')
		return (ft_putstr_fd("bad pipe\n", 2), exit_status(2, PUSH), 1);
	i = -1;
	while (s[++i])
	{
		if (s[i] == '\'' || s[i] == '\"')
			i = skip_quote(s, i);
		else if (s[i] == '|' || s[i] == '<' || s[i] == '>')
		{
			if (check_operator(s, i))
				return (ft_putstr_fd("bad op\n", 2), exit_status(2, PUSH), 1);
			i = skip_op_and_space(s, i);
			if (!s[i] || s[i] == '|' || s[i] == '<' || s[i] == '>')
				return (ft_putstr_fd("bad arg\n", 2), exit_status(2, PUSH), 1);
			i--;
		}
	}
	return (0);
}
