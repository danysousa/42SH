/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbenjami <rbenjami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/03/10 13:48:37 by rbenjami          #+#    #+#             */
/*   Updated: 2014/03/27 19:33:14 by rbenjami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"

void	sup_quote(char **token)
{
	static int		i[5] = {0, 0, 0, 0, 0};

	while ((*token)[i[0]] != '\0')
	{
		if ((*token)[i[0]] == BACK_SLASH || i[1])
		{
			if ((*token)[i[0]] == BACK_SLASH)
				i[0]++;
			i[1] = !i[1];
		}
		if ((*token)[i[0]] == DOUBLE_QUOTE)
			i[2] = (i[3]) ? i[2] : !i[2];
		if ((*token)[i[0]] == QUOTE)
			i[3] = (i[2]) ? i[3] : !i[3];
		if (!is_quote((*token)[i[0]])
		|| ((*token)[i[0]] == DOUBLE_QUOTE && i[3])
		|| ((*token)[i[0]] == QUOTE && i[2]) || i[1])
			(*token)[i[4]++] = (*token)[i[0]];
		i[0]++;
	}
	while ((*token)[i[4]] != '\0')
		(*token)[i[4]++] = '\0';
}

void	missing_quote(int dquote, t_token *token)
{
	char	*line;

	if (dquote)
	{
		g_handler.len = 8;
		ft_putstr("\033[31mdquote>\033[m ");
	}
	else
	{
		g_handler.len = 7;
		ft_putstr("\033[31mquote>\033[m ");
	}
	line = reader(IN, g_handler.hist);
	ft_strjoin2(&token->value, "\n");
	ft_strjoin2(&token->value, line);
	free(line);
	parse_string(&token);
	g_handler.len = 0;
}

void	parse(t_token *token)
{
	int		i;
	int		dquote;
	int		quote;

	i = 0;
	dquote = 0;
	quote = 0;
	while (token->value[i] != '\0')
	{
		if (token->value[i] == BACK_SLASH)
			i++;
		else
		{
			if (token->value[i] == DOUBLE_QUOTE)
				dquote = (quote) ? dquote : !dquote;
			if (token->value[i] == QUOTE)
				quote = (dquote) ? quote : !quote;
		}
		i++;
	}
	if (dquote || quote)
		missing_quote(dquote, token);
	else
		sup_quote(&token->value);
}

int		parse_string(t_token **token)
{
	t_token		*tmp;

	tmp = (*token);
	while (tmp)
	{
		if (tmp->type == STRING)
			parse(tmp);
		else if (tmp->prio == -1)
			return (error("42sh: parse error: %s\n", tmp->value));
		tmp = tmp->next;
	}
	return (0);
}
