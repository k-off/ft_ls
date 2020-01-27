/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_info.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

static size_t	get_children(t_obj *obj, int optns, int depth,
							size_t *max_lengths)
{
	DIR				*stream;
	struct dirent	*cur_child;
	size_t			res;

	res = 0;
	stream = opendir(obj->path);
	if (!stream)
		ft_printf("ft_ls: %s: Permission denied\n\0", obj->name);
	if (!stream)
		return (0);
	obj->child = (t_obj*)ft_memalloc(sizeof(t_obj) + 1);
	obj->child->depth = depth;
	cur_child = readdir(stream);
	while (cur_child)
	{
		get_names(obj->child, cur_child->d_name, obj->path);
		cur_child = readdir(stream);
	}
	closedir(stream);
	res = get_info(obj->child, optns, depth, max_lengths);
	return (res);
}

size_t			get_info(t_obj *obj, int optns, int depth, size_t *max_len)
{
	t_obj			*tmp;
	size_t			res;

	res = 0;
	tmp = obj;
	lstat(tmp->name, &tmp->val);
	while (tmp)
	{
		tmp->depth = depth;
		lstat(tmp->path, &tmp->val);
		if (optns & 2 || tmp->name[0] != '.')
			res += tmp->val.st_blocks;
		if (!tmp->print)
			tmp->print = (t_print*)ft_memalloc(sizeof(t_print) + 1);
		set_print(tmp->print, tmp->val, tmp->path);
		if (S_ISDIR(tmp->val.st_mode))
			if (tmp->depth < 1 ||
				(optns & 8 && ft_strcmp(".\0", tmp->name) != 0 &&
				ft_strcmp("..\0", tmp->name) != 0))
				tmp->child_size = get_children(tmp, optns, depth + 1, max_len);
		tmp = tmp->next;
	}
	return (res);
}
