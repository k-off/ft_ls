/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ls.h                                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LS_H
# define __LS_H

# include "Libft/libft.h"
# include <sys/syslimits.h>
# include <dirent.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>

typedef struct		s_print
{
	char			*chmod;
	char			*links_qty;
	char			*user;
	char			*group;
	char			*size;
	char			date[13];
	char			*link;
}					t_print;

typedef struct		s_obj
{
	char			*path;
	char			*name;
	size_t			depth;
	struct stat		val;
	t_print			*print;
	size_t			child_size;
	struct s_obj	*child;
	struct s_obj	*next;
}					t_obj;

typedef struct		s_data
{
	int				optns;
	t_obj			*obj;
	size_t			*max_length;
	int				qty;
}					t_data;

/*
** get_arguments.c
*/
int					get_args(t_data *data, char **argv, int argc);
void				get_names(t_obj *obj, char *name, char *path_prefix);

/*
** get_files_info.c
*/
size_t				get_info(t_obj *obj, int optns, int	depth, \
							size_t *max_lengths);

/*
** set_print_strings.c
*/
void				set_print(t_print *print, struct stat val, char *path);
size_t				*save_max(t_print *print);

/*
** print_info.c
*/
void				print_info(t_obj *objs, size_t qty, int optns, size_t *max);

/*
** sort.c
*/
t_obj				*sort_objs(t_obj *objs, int rev, int by_time);

#endif
