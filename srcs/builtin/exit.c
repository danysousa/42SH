/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbenjami <rbenjami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/03/10 18:45:41 by dsousa            #+#    #+#             */
/*   Updated: 2014/03/27 15:37:31 by rbenjami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"

int		builtin_exit(char **av)
{
	t_hist		*h;
	int			fd;
	char		*tmp;
	char		*src;

	tmp = ft_getenv("HOME");
	src = ft_strjoin(tmp, "/.42sh_history");
	h = g_handler.hist->start;
	fd = open(src, O_WRONLY | O_APPEND);
	while (h && fd > -1)
	{
		if (h->new != -1)
			ft_putendl_fd(h->data, fd);
		h = h->next;
	}
	free(tmp);
	free(src);
	if (fd > -1)
		close(fd);
	if (av && av[1])
		exit(ft_atoi(av[1]));
	else
		exit(0);
}
