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

static void	set_permissions(t_print *print, struct stat val, size_t *len)
{
	print->chmod = (char*)ft_memalloc(sizeof(char) * 11);
	if (!print->chmod)
		exit(!!ft_printf("ft_ls: error: malloc failed on permissions\n"));
	print->chmod[0] = (S_ISDIR(val.st_mode) ? 'd' : '-');
	print->chmod[0] = (S_ISLNK(val.st_mode) ? 'l' : print->chmod[0]);
	print->chmod[1] = ((val.st_mode & S_IRUSR) ? 'r' : '-');
	print->chmod[2] = ((val.st_mode & S_IWUSR) ? 'w' : '-');
	print->chmod[3] = ((val.st_mode & S_IXUSR) ? 'x' : '-');
	print->chmod[4] = ((val.st_mode & S_IRGRP) ? 'r' : '-');
	print->chmod[5] = ((val.st_mode & S_IWGRP) ? 'w' : '-');
	print->chmod[6] = ((val.st_mode & S_IXGRP) ? 'x' : '-');
	print->chmod[7] = ((val.st_mode & S_IROTH) ? 'r' : '-');
	print->chmod[8] = ((val.st_mode & S_IWOTH) ? 'w' : '-');
	print->chmod[9] = ((val.st_mode & S_IXOTH) ? 'x' : '-');
	*len = ft_strlen(print->chmod);
}

static void	set_number(size_t *number, size_t val, size_t *len)
{
	size_t	i;

	i = 0;
	*number = val;
	while (val > 0)
	{
		val /= 10;
		i++;
	}
	*len = i;
}

static void	set_user_group(t_print *print, struct stat val, \
					   size_t *len1, size_t *len2)
{
	struct passwd	*pwd;
	struct group	*gr;

	pwd = getpwuid(val.st_uid);
	if (pwd)
	{
		print->user = ft_strdup(pwd->pw_name);
		*len1 = ft_strlen(print->user);
	}
	else
		print->user = ft_strdup("");
	gr = getgrgid(val.st_gid);
	if (gr)
	{
		print->group = ft_strdup(gr->gr_name);
		*len2 = ft_strlen(print->group);
	}
	else
		print->group = ft_strdup("");
}

static void	set_date(char *date, time_t file_time, size_t *len)
{
	static	time_t	cur_time = 0;
	char			*file_time_str;

	while (cur_time < 1)
		cur_time = time(NULL);
	if (!date)
		exit(!!ft_printf("error: malloc failed on date string\n"));
	file_time_str = ctime(&file_time);
	file_time_str[24] = 0;
	ft_memcpy(&date[0], &file_time_str[4], 7);
	if (cur_time - file_time < 15778476 && cur_time - file_time > -15778476)
		ft_memcpy(&date[7], &file_time_str[11], 5);
	else
		ft_memcpy(&date[7], &file_time_str[19], 5);
}

size_t		*set_print(t_print *print, struct stat val, char *path)
{
	size_t		*lengths;
	char		*tmp;
	size_t		link_len;

	lengths = (size_t*)ft_memalloc(sizeof(size_t) * 5);
	set_permissions(print, val, &lengths[0]);
	set_number(&print->links_qty, val.st_nlink, &lengths[1]);
	set_user_group(print, val, &lengths[2], &lengths[3]);
	set_number(&print->size, val.st_size, &lengths[4]);
	print->date = (char*)ft_memalloc(sizeof(char) * 13);
	set_date(print->date, val.st_mtimespec.tv_sec, &lengths[5]);
	if (print->chmod[0] == 'l')
	{
		tmp = (char*)ft_memalloc(sizeof(char) * (print->size + 1));
		link_len = readlink(path, tmp, print->size);
		tmp[link_len] = 0;
		print->link = ft_strjoin(" -> ", tmp);
		free(tmp);
	}
	else
		print->link = ft_strdup("");
//	ft_printf("%u %u %u %u %u\n", lengths[0], lengths[1], lengths[2], lengths[3], lengths[4]);
	return (lengths);
}
