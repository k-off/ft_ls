/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_args.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

void		get_names(t_obj *obj, char *name, char *path_prefix)
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
	if (!tmp->print)
		tmp->print = (t_print*)ft_memalloc(sizeof(t_print) + 1);
	tmp->name = ft_strdup(name);
	if (obj->depth > 0)
	{
		str = path_prefix[ft_strlen(path_prefix) - 1] != '/' ?
			ft_strjoin(path_prefix, "/\0") : ft_strdup(path_prefix);
		tmp->path = ft_strjoin(str, name);
		free(str);
	}
	else
		tmp->path = ft_strdup(name);
}

static void	get_options(int *options, char *argument)
{
	size_t		len;
	size_t		i;
	size_t		j;
	int			*larrt;

	larrt = (int[5]){1, 2, 4, 8, 16};
	len = ft_strlen(argument);
	if (len < 2)
		ft_printf("ft_ls: cannot access '-': No such file or directory\n\0");
	i = 1;
	while (i < len)
	{
		if (!ft_strchr("larRt\0", argument[i]))
		{
			ft_printf("ft_ls: illegal option -- '%c'\n\0", argument[i]);
			exit(ft_printf("usage: ft_ls [-larRt] [file ...]\n\0"));
		}
		j = 0;
		while (j < 5 && argument[i] != "larRt\0"[j])
			j++;
		*options |= larrt[j];
		i++;
	}
}

int			get_args(t_data *data, char **argv, int argc)
{
	int		i;

	data->obj = (t_obj*)ft_memalloc(sizeof(t_obj) + 1);
	data->qty = 0;
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
		data->qty = 1;
		data->obj->name = ft_strdup(".\0");
		data->obj->path = ft_strdup(".\0");
	}
	return (1);
}
