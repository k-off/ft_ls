/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   sort_info.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

static void		split_objs(t_obj *obj, t_obj **left, t_obj **right, size_t len)
{
	size_t	left_half;
	size_t	i;
	t_obj	*tmp;

	left_half = len / 2 + len % 2;
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

static int		in_order(t_obj *left, t_obj *right, int reverse, int by_time)
{
	if (!left)
		return (0);
	if (!right)
		return (1);
	if (by_time)
		return (((left->val.st_mtimespec.tv_sec -
				right->val.st_mtimespec.tv_sec) * reverse > 0) ||
				(((left->val.st_mtimespec.tv_sec -
				right->val.st_mtimespec.tv_sec) * reverse == 0) &&
				(strcmp(left->name, right->name) * reverse <= 0)));
	else
		return (strcmp(left->name, right->name) * reverse <= 0);
}

static t_obj	*merge_objs(t_obj *left, t_obj *right, int rev, int by_time)
{
	t_obj	*merged;
	t_obj	*tmp;

	tmp = in_order(left, right, rev, by_time) ? left : right;
	left = (tmp == left) ? left->next : left;
	right = (tmp == right) ? right->next : right;
	merged = tmp;
	while (left || right)
	{
		tmp->next = in_order(left, right, rev, by_time) ? left : right;
		tmp = tmp->next;
		left = (tmp == left && left != NULL) ? left->next : left;
		right = (tmp == right && right != NULL) ? right->next : right;
	}
	tmp->next = 0;
	return (merged);
}

t_obj			*sort_objs(t_obj *objs, int rev, int by_time)
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
	if (len == 1 && objs->child)
		objs->child = sort_objs(objs->child, rev, by_time);
	else if (len > 1)
	{
		split_objs(tmp, &left, &right, len);
		left = sort_objs(left, rev, by_time);
		right = sort_objs(right, rev, by_time);
		tmp = merge_objs(left, right, rev, by_time);
	}
	return (tmp);
}
