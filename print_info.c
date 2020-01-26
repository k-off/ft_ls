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

static void	print_all(t_obj *tmp, size_t qty, int optns)
{
	static char	*o = NULL;
	size_t		*m;

	if (!o)
		o = ft_strdup("                                        ");
	m = save_max(NULL);
	ft_printf("%s", tmp->print->chmod);
	ft_printf(" %s", &o[38 - (m[0] - ft_strlen(tmp->print->links_qty))]);
	ft_printf(" %s", tmp->print->links_qty);
	ft_printf(" %s", &o[38 - (m[1] - ft_strlen(tmp->print->user))]);
	ft_printf(" %s", tmp->print->user);
	ft_printf(" %s", &o[38 - (m[2] - ft_strlen(tmp->print->group))]);
	ft_printf(" %s", tmp->print->group);
	ft_printf(" %s", &o[38 - (m[3] - ft_strlen(tmp->print->size))]);
	ft_printf(" %s", tmp->print->size);
	ft_printf(" %s", tmp->print->date);
	ft_printf(" %s", tmp->name);
	if (tmp->print->chmod[0] == 'l')
		ft_printf("%s", tmp->print->link);
}

static void	print_object(t_obj *tmp, size_t qty, int optns)
{
	
	
	if ((optns & 2) == 0 && tmp->name[0] == '.')
		return ;
	if ((optns & 1))
		print_all(tmp, qty, optns);
	else
		ft_printf("%s", tmp->name);
	if (optns & 1 || tmp->next == NULL)
		ft_printf("\n");
	else
		ft_printf("\t");
}


//static void	set_offsets(t_obj *obj, size_t *max)
//{
//	size_t	len;
//	size_t	i;
//	static char	*offset = "                                        ";
//
//	i = 0;
//	//obj->print->offsets = (char**)ft_memalloc(sizeof(char*) * 6);
//	while (i < 5)
//	{
//		len = 0;
//		if (obj->lengths[i] <= max[i])
//			len = max[i] - obj->lengths[i];
//		if (len > 40)
//			len = 0;
//		obj->print->offsets[i] = ft_strdup(&offset[38 - len]);
//		i++;
//	}
//	ft_printf("%u %u %u %u %u\n", obj->lengths[0], obj->lengths[1], obj->lengths[2], obj->lengths[3], obj->lengths[4]);
//	ft_printf("%u %u %u %u %u\n", max[0], max[1], max[2], max[3], max[4]);
//}

void		print_objects(t_obj *objs, size_t qty, int optns)
{
	t_obj		*tmp;

	tmp = objs;
	while (tmp && !tmp->depth)
	{
		if (tmp->print->chmod[0] != 'd')
		{
			if (tmp->val.st_ctimespec.tv_sec)
				print_object(tmp, qty, optns);
		}
		tmp = tmp->next;
	}
	tmp = objs;
	while (tmp && tmp->depth)
	{
		if (tmp->val.st_ctimespec.tv_sec)
			print_object(tmp, qty, optns);
		tmp = tmp->next;
	}
}

//void		print_depth_next(t_obj *objs, size_t qty, int optns, size_t *max)
//{
//	t_obj		*tmp;
//
//	tmp = objs;
//	while (tmp)
//	{
//		if (tmp->val.st_ctimespec.tv_sec)
//			print_object(tmp, qty, optns, max);
//		tmp = tmp->next;
//	}
//}

void		print_info(t_obj *objs, size_t qty, int optns, size_t *max)
{
	t_obj		*tmp;
	size_t		i;

	tmp = objs;
	i = 0;
	if (objs)
		print_objects(objs, qty, optns);
	tmp = objs;
	while (tmp && (optns & 8 || tmp->depth == 0))
	{
		if (tmp->child)
		{
			(i > 0 || tmp->depth > 0) ? ft_printf("\n") : ft_printf("");
			if (((qty > 0 && tmp->depth == 0) || tmp->depth != 0))
				ft_printf("%s:\n", tmp->path);
			if (optns & 1)
				ft_printf("total %d\n", tmp->child_size);
			print_info(tmp->child, qty, optns, max);
			i++;
		}
		tmp = tmp->next;
	}
}
