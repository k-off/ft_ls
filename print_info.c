/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   print_info.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

static void	print_all(t_obj *tmp)
{
	static char	*offset = NULL;
	size_t		*max;

	if (!offset)
		offset = ft_strdup("                                        \0");
	max = save_max(NULL);
	ft_printf("%s\0", tmp->print->chmod);
	ft_printf("%s\0", &offset[38 -
						(max[0] - ft_strlen(tmp->print->links_qty))]);
	ft_printf("%s\0", tmp->print->links_qty);
	ft_printf("%s\0", &offset[39 - (max[1] - ft_strlen(tmp->print->user))]);
	ft_printf("%s\0", tmp->print->user);
	ft_printf("%s\0", &offset[38 - (max[2] - ft_strlen(tmp->print->group))]);
	ft_printf("%s\0", tmp->print->group);
	ft_printf("%s\0", &offset[38 - (max[3] - ft_strlen(tmp->print->size))]);
	ft_printf("%s\0", tmp->print->size);
	ft_printf(" %s\0", tmp->print->date);
	ft_printf(" %s\0", tmp->name);
	if (tmp->print->chmod[0] == 'l')
		ft_printf("%s\0", tmp->print->link);
}

static void	print_object(t_obj *tmp, int optns)
{
	if ((optns & 2) == 0 && tmp->name[0] == '.')
		return ;
	if ((optns & 1))
		print_all(tmp);
	else
		ft_printf("%s\0", tmp->name);
	if (optns & 1 || tmp->next == NULL)
		ft_printf("\n\0");
	else
		ft_printf("\t\0");
}

void		print_objects(t_obj *objs, int optns)
{
	t_obj		*tmp;

	tmp = objs;
	while (tmp && !tmp->depth)
	{
		if (tmp->print->chmod[0] != 'd')
		{
			if (tmp->val.st_ctimespec.tv_sec)
				print_object(tmp, optns);
		}
		tmp = tmp->next;
	}
	tmp = objs;
	while (tmp && tmp->depth)
	{
		if (tmp->val.st_ctimespec.tv_sec)
			print_object(tmp, optns);
		tmp = tmp->next;
	}
}

void		print_info(t_obj *objs, size_t qty, int optns, size_t *max)
{
	t_obj		*tmp;
	size_t		i;

	tmp = objs;
	i = 0;
	if (objs)
		print_objects(objs, optns);
	tmp = objs;
	while (tmp && (optns & 8 || tmp->depth == 0))
	{
		if (tmp->child)
		{
			(i > 0 || tmp->depth > 0) ? ft_printf("\n\0") : ft_printf("\0");
			if (((qty > 0 && tmp->depth == 0) || tmp->depth != 0))
				ft_printf("%s:\n\0", tmp->path);
			if (optns & 1)
				ft_printf("total %d\n\0", tmp->child_size);
			print_info(tmp->child, qty, optns, max);
			i++;
		}
		tmp = tmp->next;
	}
}
