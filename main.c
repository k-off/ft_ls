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

int     main(int argc, char **argv)
{
    t_args      *args;
    t_res       *res;

    args = 0;
    res = 0;
    get_args(&args, argv, argc);
    get_res(&res, args);
    //print_res(args->optns, res);
    return (0);
}