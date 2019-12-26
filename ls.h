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
# include <dirent.h>

# define 			larRt (int[5]){1, 2, 4, 8, 16}

typedef struct		s_args
{
	int				optns;
	char			**names;
}					t_args;

typedef struct		s_res
{
	struct s_res	*prev;
	struct stat		value;
	struct s_res	*next;
}					t_res;

int					get_args(t_args **args, char **argv, int argc);

#endif