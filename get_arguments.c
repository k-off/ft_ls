/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_arguments.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

void	get_names(t_obj *obj, char *name, char *path_prefix)
{
	t_obj		*tmp;
	char		*str;

	tmp = obj;
	while (tmp->next && tmp->name)
		tmp = tmp->next;
	if (tmp->name)
	{
		tmp->next = (t_obj*)ft_memalloc(sizeof(t_obj));
		tmp = tmp->next;
	}
	tmp->name = ft_strdup(name);
	str = ft_strjoin(path_prefix, "/");
	tmp->path = ft_strjoin(str, name);
	free(str);
}

static void	get_options(int *options, char *argument)
{
	size_t	len;
	int		i;
	int		j;

	len = ft_strlen(argument);
	if (len < 2)
		ft_printf("ft_ls: cannot access '-': No such file or directory\n");
	i = 1;
	while (i < len)
	{
		if (!ft_strchr("larRt", argument[i]))
		{
			ft_printf("ls: illegal option -- '%c'\n", argument[i]);
			exit (ft_printf("usage: ft_ls [-larRt] [file ...]"));
		}
		j = 0;
		while (j < 5 && argument[i] != "larRt"[j])
			j++;
		*options |= larRt[j];
		i++;
	}
}

int     	get_args(t_data *data, char **argv, int argc)
{
	int		i;

	data->obj = (t_obj*)ft_memalloc(sizeof(t_obj));
	data->qty = 0;
	data->optns = 0;
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-')
			get_options(&data->optns, argv[i]);
		else
		{
			get_names(data->obj, argv[i], argv[i]);
			data->qty++;
		}
		i++;
	}
	if (data->qty < 1)
	{
		data->obj->name = ft_strdup(".");
		data->obj->path = ft_strdup(".");
		data->qty = 1;
	}
    return (1);
}
