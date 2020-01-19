/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   set_print_strings.c                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

void	get_children(t_obj *obj, int optns, int depth, size_t *max_lengths)
{
	DIR				*stream;
	struct dirent	*cur_child;

	stream = opendir(obj->path);
	obj->child = (t_obj*)ft_memalloc(sizeof(t_obj) + 1);
	cur_child = readdir(stream);
	while (cur_child)
	{
		get_names(obj->child, cur_child->d_name, obj->path);
		cur_child = readdir(stream);
	}
	closedir(stream);
	if (optns & 8 || depth < 2)
		get_info(obj->child, optns, depth, max_lengths);
}

void	check_lengths(size_t *cur_lengths, size_t max_lengths[5])
{
	int				i;

	i = 0;
	while (i < 5)
	{
		if (cur_lengths[i] > max_lengths[i])
			max_lengths[i] = cur_lengths[i];
		i++;
	}
}

void	get_info(t_obj *obj, int optns, int	depth, size_t max_lengths[5])
{
	t_obj			*tmp;

	tmp = obj;
	lstat(tmp->name, &tmp->val);
	while (tmp)
	{
		tmp->depth = depth;
		lstat(tmp->path, &tmp->val);
		tmp->print = (t_print*)ft_memalloc(sizeof(t_print) + 1);
		check_lengths(set_print(tmp->print, tmp->val, tmp->path), max_lengths);
		if (S_ISDIR(tmp->val.st_mode))
			if (((ft_strcmp(".", tmp->name) == 0 ||
				 ft_strcmp("..", tmp->name) == 0) && tmp->depth < 1) ||
				(ft_strcmp(".", tmp->name) != 0 &&
				ft_strcmp("..", tmp->name) != 0)
				)
			get_children(tmp, optns, depth + 1, max_lengths);
		tmp = tmp->next;
	}
}
