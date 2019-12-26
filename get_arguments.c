/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_arguments.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

static void	get_names(char **names, char *name)
{
	static int		i = 0;

	if (ft_strlen(name) < 1)
	{
		ft_printf("ft_ls: cannot access '': No such file or directory\n");
		exit (1);
	}
	names[i] = ft_strdup(name);
	i++;
}

static void	get_options(int *options, char *argument)
{
	int		len;
	int		i;
	int		j;

	len = ft_strlen(argument);
	if (len < 2)
	{
		ft_printf("ft_ls: cannot access '-': No such file or directory\n");
		exit (1);
	}
	i = 1;
	while (i < len)
	{
		if (!ft_strchr("larRt", argument[i]))
			exit (ft_printf("ls: invalid option -- '%c'\n", argument[i]));
		j = 0;
		while (j < 5 && argument[i] != "larRt"[j])
			j++;
		*options |= larRt[j];
		i++;
	}
}

int     	get_args(t_args **args, char **argv, int argc)
{
	int		i;

	*args = (t_args*)ft_memalloc(sizeof(t_args));
	if (*args == 0)
		return (0);
	args[0]->names = (char**)ft_memalloc(sizeof(char*) * argc);
	if (args[0]->names == 0)
		return (0);
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-')
			get_options(&args[0]->optns, argv[i]);
		else
			get_names(args[0]->names, argv[i]);
		i++;
	}
    return (1);
}
