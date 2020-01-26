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

int		main(int argc, char **argv)
{
	t_data		*data;

	data = (t_data*)ft_memalloc(sizeof(data) + 1);
	get_args(data, argv, argc);
	get_info(data->obj, data->optns, 0, data->max_length);
	data->obj = sort_objs(data->obj, (data->optns & 4) == 0 ? 1 : -1,
						data->optns & 16);
	if (data->qty == 1 && data->obj->print->chmod[0] == 'd')
	{
		if (data->optns & 1)
			ft_printf("total %d\n", data->obj->child_size);
		print_info(data->obj->child, data->qty, data->optns, data->max_length);
	}
	else
		print_info(data->obj, data->qty, data->optns, data->max_length);
	return (0);
}
