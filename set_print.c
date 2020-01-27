/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   set_print.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali@student.codam.nl                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                 */
/*   Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

static void	set_permissions(char **mod, mode_t md)
{
	mod[0] = (char*)ft_memalloc(sizeof(char) * 11);
	if (!mod[0])
		exit(!!ft_printf("ft_ls: error: malloc failed on permissions\n\0"));
	mod[0][0] = (S_ISDIR(md) ? 'd' : '-');
	mod[0][0] = (S_ISLNK(md) ? 'l' : mod[0][0]);
	mod[0][0] = (S_ISFIFO(md) ? 'p' : mod[0][0]);
	mod[0][0] = (S_ISBLK(md) ? 'b' : mod[0][0]);
	mod[0][0] = (S_ISCHR(md) ? 'c' : mod[0][0]);
	mod[0][0] = (S_ISSOCK(md) ? 's' : mod[0][0]);
	mod[0][1] = ((md & S_IRUSR) ? 'r' : '-');
	mod[0][2] = ((md & S_IWUSR) ? 'w' : '-');
	mod[0][3] = ((md & S_IXUSR) ? 'x' : '-');
	mod[0][3] = (((md & S_ISVTX) && (md & S_IRUSR)) ? 's' : mod[0][3]);
	mod[0][3] = (((md & S_ISVTX) && !(md & S_IRUSR)) ? 'S' : mod[0][3]);
	mod[0][4] = ((md & S_IRGRP) ? 'r' : '-');
	mod[0][5] = ((md & S_IWGRP) ? 'w' : '-');
	mod[0][6] = ((md & S_IXGRP) ? 'x' : '-');
	mod[0][6] = (((md & S_ISVTX) && (md & S_IRGRP)) ? 's' : mod[0][6]);
	mod[0][6] = (((md & S_ISVTX) && !(md & S_IRGRP)) ? 'S' : mod[0][6]);
	mod[0][7] = ((md & S_IROTH) ? 'r' : '-');
	mod[0][8] = ((md & S_IWOTH) ? 'w' : '-');
	mod[0][9] = ((md & S_IXOTH) ? 'x' : '-');
	mod[0][9] = (((md & S_ISVTX) && (md & S_IROTH)) ? 't' : mod[0][9]);
	mod[0][9] = (((md & S_ISVTX) && !(md & S_IROTH)) ? 'T' : mod[0][9]);
}

static void	set_user_group(t_print *print, struct stat val)
{
	struct passwd	*pwd;
	struct group	*gr;

	pwd = getpwuid(val.st_uid);
	if (pwd)
		print->user = ft_strdup(pwd->pw_name);
	else
		print->user = ft_strdup("\0");
	gr = getgrgid(val.st_gid);
	if (gr)
		print->group = ft_strdup(gr->gr_name);
	else
		print->group = ft_strdup("\0");
}

static void	set_date(char *date, time_t file_time)
{
	static	time_t	cur_time = 0;
	char			*file_time_str;

	while (cur_time < 1)
		cur_time = time(NULL);
	if (!date)
		exit(!!ft_printf("error: malloc failed on date string\n\0"));
	file_time_str = ctime(&file_time);
	file_time_str[24] = 0;
	ft_memcpy(&date[0], &file_time_str[4], 7);
	if (cur_time - file_time < 15778476 && cur_time - file_time > -15778476)
		ft_memcpy(&date[7], &file_time_str[11], 5);
	else
		ft_memcpy(&date[7], &file_time_str[19], 5);
}

size_t		*save_max(t_print *print)
{
	static size_t	*max_len = NULL;
	size_t			*len;

	len = (size_t*)ft_memalloc(sizeof(size_t) * 4);
	if (print)
	{
		len[0] = ft_strlen(print->links_qty);
		len[1] = ft_strlen(print->user);
		len[2] = ft_strlen(print->group);
		len[3] = ft_strlen(print->size);
	}
	if (!max_len)
		max_len = (size_t*)ft_memalloc(sizeof(size_t) * 4);
	max_len[0] = max_len[0] < len[0] && len[0] < 40 ? len[0] : max_len[0];
	max_len[1] = max_len[1] < len[1] && len[1] < 40 ? len[1] : max_len[1];
	max_len[2] = max_len[2] < len[2] && len[2] < 40 ? len[2] : max_len[2];
	max_len[3] = max_len[3] < len[3] && len[3] < 40 ? len[3] : max_len[3];
	free(len);
	return (max_len);
}

void		set_print(t_print *print, struct stat val, char *path)
{
	char		*tmp;
	size_t		link_len;

	set_permissions(&print->chmod, val.st_mode);
	print->links_qty = ft_itoa((int)val.st_nlink);
	set_user_group(print, val);
	print->size = ft_itoa((int)val.st_size);
	set_date(print->date, val.st_mtimespec.tv_sec);
	save_max(print);
	if (val.st_ctimespec.tv_sec == 0)
		ft_printf("ft_ls: %s: File couldn't be found\n\0", path);
	if (print->chmod[0] == 'l')
	{
		tmp = (char*)ft_memalloc(sizeof(char) * (val.st_size + 1));
		link_len = readlink(path, tmp, val.st_size);
		tmp[link_len] = 0;
		print->link = ft_strjoin(" -> \0", tmp);
		free(tmp);
	}
	else
		print->link = ft_strdup("\0");
}
