/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

void	split_objs(t_obj *obj, t_obj **left, t_obj **right, size_t len)
{
	size_t	left_half;
	size_t	i;
	t_obj	*tmp;

	left_half = len / 2 + len % 2;
	ft_printf("%d %d\n", len, left_half);
	i = 1;
	tmp = obj;
	while (i < left_half)
	{
		tmp = tmp->next;
		i++;
	}
	left[0] = obj;
	right[0] = tmp->next;
	tmp->next = 0;
}

t_obj	*compare_names(t_obj **obj1, t_obj **obj2, int reverse)
{
	t_obj *tmp;
	if (ft_strcmp(obj1[0]->name, obj2[0]->name) * reverse > 0)
	{
		tmp = obj2[0];
		obj2[0] = obj2[0]->next;
		return (tmp);
	}
	else
	{
		tmp = obj1[0];
		obj1[0] = obj1[0]->next;
		return (tmp);
	}
}

t_obj	*compare_dates(t_obj **obj1, t_obj **obj2, int reverse)
{
	t_obj *tmp;
	if ((obj1[0]->val.st_ctimespec.tv_sec - \
		obj2[0]->val.st_ctimespec.tv_sec) * reverse < 0)
	{
		tmp = obj2[0];
		obj2[0] = obj2[0]->next;
		return (tmp);
	}
	else
	{
		tmp = obj1[0];
		obj1[0] = obj1[0]->next;
		return (tmp);
	}
}

t_obj	*merge_objs(t_obj *left, t_obj *right, int rev, int by_time)
{
	t_obj	*merged;
	t_obj	*tmp;
	t_obj	*(*cmp_ptr[2])(t_obj**, t_obj**, int);

	cmp_ptr[0] = &compare_names;
	cmp_ptr[1] = &compare_dates;
	tmp = cmp_ptr[by_time](&left, &right, rev);
	merged = tmp;
	while (left && right)
	{
		tmp->next = cmp_ptr[by_time](&left, &right, rev);
		tmp = tmp->next;
	}
	left = (left == NULL) ? right : left;
	while (left)
	{
		tmp->next = left;
		tmp = tmp->next;
		left = left->next;
	}
	tmp->next = NULL;
	t_obj	*tmp2;
	tmp2 = merged;
	while (tmp2)
	{
		ft_printf("%s ", tmp2->name);
		tmp2 = tmp2->next;
	}
	ft_printf("\n");
	return (merged);
}

void	sort_objs(t_obj *objs, int rev, int by_time)
{
	t_obj	*left;
	t_obj	*right;
	t_obj	*tmp;
	size_t	len;

	len = 0;
	tmp = objs;
	while (tmp)
	{
		len++;
		tmp = tmp->next;
	}
	tmp = objs;
	if (len > 1)
	{
		split_objs(tmp, &left, &right, len);
//		tmp = left;
//		while (tmp)
//		{
//			ft_printf("%s ", tmp->name);
//			tmp = tmp->next;
//		}
//		ft_printf("\n");
//		tmp = right;
//		while (tmp)
//		{
//			ft_printf("%s ", tmp->name);
//			tmp = tmp->next;
//		}
//		ft_printf("\n--------------\n\n");
		sort_objs(left, rev, by_time);
		sort_objs(right, rev, by_time);
		tmp = merge_objs(left, right, rev, by_time);
	}
	else if (len < 2 && objs->child)
		sort_objs(tmp->child, rev, by_time);
	objs = tmp;
}

void	print_info(t_data *data)
{
	t_obj		*obj;

	obj = data->obj;
	while (obj)
	{
		ft_printf("%s", obj->name);
		obj = obj->next;
	}
}

int     main(int argc, char **argv)
{
	t_data		*data;

	data = (t_data*)ft_memalloc(sizeof(data) + 1);
    get_args(data, argv, argc);
	data->max_length = (size_t*)ft_memalloc(sizeof(size_t) * 5);
	get_info(data->obj, data->optns, 0, data->max_length);
	sort_objs(data->obj, (data->optns & 4) == 0 ? 1 : -1, (data->optns & 16) >> 4);
	print_info(data);
    return (0);
}
