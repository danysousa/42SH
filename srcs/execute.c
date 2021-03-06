/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbenjami <rbenjami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/03/04 15:56:25 by rbenjami          #+#    #+#             */
/*   Updated: 2014/03/27 15:37:35 by rbenjami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <dirent.h>
#include "sh.h"

int			find_arg_path(void)
{
	int				i;

	i = 0;
	while (g_handler.env[i])
	{
		if (!(ft_strncmp(g_handler.env[i], "PATH", 4)))
			return (i);
		i++;
	}
	return (-1);
}

int			is_dir(char *cmd, char *path)
{
	DIR				*rep;
	struct dirent	*lecture;

	rep = opendir(path);
	if (rep == NULL)
		return (-1);
	lecture = readdir(rep);
	while (lecture)
	{
		if (ft_strcmp(cmd, lecture->d_name) == 0)
		{
			closedir(rep);
			return (1);
		}
		lecture = readdir(rep);
	}
	closedir(rep);
	return (0);
}

char		*find_path(char *cmd, char **environ, int i)
{
	static char	**path = NULL;
	char		*res;
	char		*tmp;
	int			j;

	if (ft_strchr(cmd, '/'))
		return (cmd);
	if (i == -1)
		return (NULL);
	j = 0;
	if (!path)
		path = ft_strsplit(&environ[i][5], ':');
	while (path[j])
	{
		if (is_dir(cmd, path[j]) == 1)
		{
			tmp = ft_strjoin(path[j], "/");
			res = ft_strjoin(tmp, cmd);
			free(tmp);
			return (res);
		}
		j++;
	}
	return (NULL);
}

int			execute2(t_exe *exe, char *cmd)
{
	if (!cmd[0])
		return (error("42sh: permission denied: \n"));
	if ((exe->args = ft_strsplit_space(cmd)) == NULL)
		return (error("42sh: command not found: %s\n", cmd));
	exe->builtin = find_builtin(exe->args[0]);
	exe->path = find_path(exe->args[0], g_handler.env, find_arg_path());
	if (!exe->path && !exe->builtin)
	{
		ft_free_tab(&exe->args);
		return (error("42sh: command not found: %s\n", cmd));
	}
	if (exe->builtin && (exe->ret = ft_isfuncfork(exe->args[0])) == 0)
	{
		if (exe->builtin(exe->args) < 0)
		{
			if (exe->path && exe->path[0])
				free(exe->path);
			ft_free_tab(&exe->args);
			return (-1);
		}
	}
	return (1);
}

pid_t		execute(char *cmd, int	pfd_old[2], int	pfd[2], int b)
{
	t_exe	exe;

	if (execute2(&exe, cmd) != 1)
		return (-42);
	if ((exe.pid = fork()) < 0)
		return (error("42sh: fork error !\n"));
	turn_off(g_handler.term);
	if (exe.pid == 0)
	{
		if (pfd || pfd_old)
			dup_close(pfd, pfd_old, b);
		if (exe.builtin)
		{
			if (exe.ret && exe.builtin(exe.args) < 0)
				exit(1);
			exit(0);
		}
		else if (execve(exe.path, exe.args, g_handler.env) == -1)
			exit(1);
	}
	ft_free_tab(&exe.args);
	if (exe.path && exe.path[0])
		free(exe.path);
	return (exe.pid);
}
