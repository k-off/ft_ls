/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   unit_test.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgross <mgross@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/04/11 10:42:20 by mgross         #+#    #+#                */
/*   Updated: 2019/07/20 22:22:53 by krioliin      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <float.h>
#include "../ft_printf.h"

# define BOLD			"\x1B[1m"
# define ITALICS		"\x1B[3m"
# define UNDERLINE		"\x1B[4m"
# define INVERSE		"\x1B[7m"
# define BOLD_OFF		"\x1B[22m"
# define ITALICS_OFF	"\x1B[23m"
# define UNDERLINE_OFF	"\x1B[24m"
# define INVERSE_OFF	"\x1B[27m"
# define BLACK 			"\x1B[30m"
# define RED 			"\x1B[31m"
# define GREEN 			"\x1B[32m"
# define YELLOW 		"\x1B[33m"
# define BLUE 			"\x1B[34m"
# define MAGENTA 		"\x1B[35m"
# define CYAN 			"\x1B[36m"
# define WHITE 			"\x1B[37m"
# define EOC 			"\e[0m"

/*
** *****************************GET_NEXT_LINE***********************************
*/

typedef struct			s_fd_list
{
	int					fd;
	char				*tail;
	struct s_fd_list	*next;
}						t_fd_list;

typedef struct          s_sline
{
	int                 fd;
	char                *str;
	struct s_sline       *next;
}                       t_tline;

t_fd_list	*creat_elem(int fd);
t_fd_list	*find_fd(t_fd_list **top, int fd);
int			fd_read(t_fd_list **elem, ssize_t size);
int			gnl_concat(const int fd, char **line);

# define BUFF_SIZE 100

t_fd_list	*creat_elem(int fd)
{
	t_fd_list *new_elem;
	
	new_elem = (t_fd_list *)malloc(sizeof(t_fd_list));
	if (!new_elem)
		return (NULL);
	new_elem->fd = fd;
	new_elem->tail = ft_strnew(0);
	new_elem->next = NULL;
	return (new_elem);
}

t_fd_list	*find_fd(t_fd_list **top, int fd)
{
	t_fd_list *head;
	
	if (!(*top))
	{
		*top = creat_elem(fd);
		return (*top);
	}
	head = *top;
	while (head->next != NULL)
	{
		if (head->fd == fd)
			return (head);
		head = head->next;
	}
	if (head->fd == fd)
		return (head);
	head->next = creat_elem(fd);
	return (head->next);
}

int			fd_read(t_fd_list **elem, ssize_t size)
{
	ssize_t	byte;
	char	buff[size + 1];
	char	*temp;
	
	ft_bzero(buff, size + 1);
	if (ft_strchr((*elem)->tail, '\n') > -1)
		return (1);
	while ((byte = read((*elem)->fd, buff, BUFF_SIZE)) >= 0)
	{
		if (buff[0] != '\0')
		{
			if (byte < BUFF_SIZE)
				buff[byte] = '\0';
			temp = (*elem)->tail;
			(*elem)->tail = ft_strjoin((*elem)->tail, buff);
			free(temp);
			if (ft_strchr(buff, '\n') || byte == 0)
				return (1);
		}
		if (ft_strlen((*elem)->tail) && byte == 0)
			return (1);
		if (byte == 0)
			return (0);
	}
	return (-1);
}

int			gnl_concat(const int fd, char **line)
{
	static t_fd_list	*top;
	t_fd_list			*elem;
	int					end;
	int					ret;
	int					len;
	
	elem = find_fd(&top, fd);
	if ((ret = fd_read(&elem, BUFF_SIZE)) > -1)
	{
		if ((end = ft_strchr(elem->tail, '\n') - elem->tail) > -1)
		{
			*line = ft_strsub(elem->tail, 0, end);
			end += 1;
			len = ft_strlen(&elem->tail[end]);
			ft_memmove((void *)elem->tail, (const void *)elem->tail + end, len);
			elem->tail[len] = '\0';
		}
		else if ((end = ft_strchr(elem->tail, '\0') - elem->tail) > -1)
		{
			*line = ft_strsub(elem->tail, 0, end);
			if (elem->tail)
				ft_bzero(elem->tail, ft_strlen(elem->tail));
		}
	}
	return (ret);
}

int			ft_get_next_line(const int fd, char **line)
{
	if (fd < 0 || !line || BUFF_SIZE < 1)
		return (-1);
	*line = NULL;
	return (gnl_concat(fd, line));
}

/*
** *****************************************************************************
*/

int stdout_save;
char buffer[2048];

void		stream_to_file(int *stdout_save, char *buffer)
{
	fflush(stdout); //clean everything first
	*stdout_save = dup(STDOUT_FILENO); //save the stdout state 
	freopen("NUL", "a", stdout); //redirect stdout to null pointer
	setvbuf(stdout, buffer, _IOFBF, 1024); //set buffer to stdout
}

int			wordlen(char *s)
{
	int		i;

	i = 0;
	while (s[i] != '[' && s[i] != 0)
		i++;
	return (i);
}

void		check_equal(int *stdout_save, char less)
{
	freopen("NUL", "a", stdout); //redirect stdout to null again
	dup2(*stdout_save, STDOUT_FILENO); //restore the previous state of stdout
	setvbuf(stdout, NULL, _IONBF, 1024); //disable buffer to print to screen instantly
	
	char *line1;
	char *line2;
	int fd;
	fd = open("NUL", O_RDONLY);
	ft_get_next_line(fd, &line1);
	ft_get_next_line(fd, &line2);
	//line2 = "test";	// uncomment als je fail output wil testen
	if (ft_strequ(ft_strchr(line1, '['), ft_strchr(line2, '[')))
	{
		printf("\033[1;32m	[SUCCES]\033[0m\n");
		if (!less)
		{
			printf("%s\033[1;30m%s\033[0m\n", ft_strsub(line1, 0, wordlen(line1)), ft_strchr(line1, '['));
			printf("%s\033[1;30m%s\033[0m\n", ft_strsub(line2, 0, wordlen(line2)), ft_strchr(line2, '['));
		}
	}
	else
	{
		
		printf("\033[1;31m	[FAIL]\033[0m\n");
		if (!less)
		{
			printf("%s\033[1;30m%s\033[0m\n", ft_strsub(line1, 0, wordlen(line1)), ft_strchr(line1, '['));
			printf("%s\033[1;30m%s\033[0m\n", ft_strsub(line2, 0, wordlen(line2)), ft_strchr(line2, '['));
		}
	}
	printf(EOC);
	remove("NUL");
}

void	unit_specifier_c(char less)
{
	static done = 0;

	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier c]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [c]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%c]\n", 'a');
		ft_printf("ft_printf:	[%c]\n", 'a');
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][c]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10c]\n", 'a');
		ft_printf("ft_printf:	[%10c]\n", 'a');
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][c]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*c]\n", 10, 'a');
		ft_printf("ft_printf:	[%*c]\n", 10, 'a');
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10][c]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10c]\n", 'a');
		ft_printf("ft_printf:	[%-10c]\n", 'a');
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][c]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*c]\n", 10, 'a');
		ft_printf("ft_printf:	[%-*c]\n", 10, 'a');
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][c] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*c]\n", -10, 'a');
		ft_printf("ft_printf:	[%*c]\n", -10, 'a');
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_s(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 0;
		
		printf("\n\033[1;30m----[Test for Specifier s]----\033[0m\n");
		if (!less)
		printf("----[Test %i]----\n", i++);
		printf("Specifier   = [s]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%s]\n", "Dit is een test");
		ft_printf("ft_printf:	[%s]\n", "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][s]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10s]\n", "Dit");
		ft_printf("ft_printf:	[%10s]\n", "Dit");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][s] (String longer then width)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10s]\n", "Dit is een test");
		ft_printf("ft_printf:	[%10s]\n", "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][s]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20s]\n", "Dit is een test");
		ft_printf("ft_printf:	[%-20s]\n", "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][.5][s]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20.5s]\n", "Dit is een test");
		ft_printf("ft_printf:	[%-20.5s]\n", "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [20][.5][s]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.5s]\n", "Dit is een test");
		ft_printf("ft_printf:	[%20.5s]\n", "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [20][.5][s]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.*s]\n", 5, "Dit is een test");
		ft_printf("ft_printf:	[%20.*s]\n", 5, "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][s]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*s]\n", 20, "Dit is een test");
		ft_printf("ft_printf:	[%*s]\n", 20, "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][s] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*s]\n", -20, "Dit is een test");
		ft_printf("ft_printf:	[%*s]\n", -20, "Dit is een test");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][s] (String longer then width)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*s]\n", 10, "Dit is een test");
		ft_printf("ft_printf:	[%-*s]\n", 10, "Dit is een test");
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_di(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 0;
		
		printf("\n\033[1;30m----[Test for Specifier di]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-d]\n", 42);
		ft_printf("ft_printf:	[%-d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-d]\n", 0);
		ft_printf("ft_printf:	[%-d]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][0][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-010d]\n", 0);
		ft_printf("ft_printf:	[%-010d]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10d]\n", 42);
		ft_printf("ft_printf:	[%-10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+10d]\n", 42);
		ft_printf("ft_printf:	[%-+10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][ ][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+ 10d]\n", 42);
		ft_printf("ft_printf:	[%-+ 10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.10d]\n", 42);
		ft_printf("ft_printf:	[%-.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+ .10d]\n", 42);
		ft_printf("ft_printf:	[%-+ .10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-5.10d]\n", 42);
		ft_printf("ft_printf:	[%-5.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10d]\n", 42);
		ft_printf("ft_printf:	[%-10.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][15][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-15.10d]\n", 42);
		ft_printf("ft_printf:	[%-15.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][0][10][.5][d]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+010.5d]\n", 42);
		ft_printf("ft_printf:	[%-+010.5d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----HELLOOOOOOOOOOOOOO\n", i++);
		printf("Specifier   = [-][+][0][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.15d]\n", 42);
		ft_printf("ft_printf:	[%-10.15d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-d]\n", -42);
		ft_printf("ft_printf:	[%-d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10d]\n", -42);
		ft_printf("ft_printf:	[%-10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+10d]\n", -42);
		ft_printf("ft_printf:	[%-+10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----  I am a test!!!!!!!\n", i++);
		printf("Specifier   = [-][+][ ][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+ 10d]\n", -42);
		ft_printf("ft_printf:	[%-+ 10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.10d]\n", -42);
		ft_printf("ft_printf:	[%-.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][ ][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+ .10d]\n", -42);
		ft_printf("ft_printf:	[%-+ .10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-5.10d]\n", -42);
		ft_printf("ft_printf:	[%-5.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10d]\n", -42);
		ft_printf("ft_printf:	[%-10.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][15][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-15.10d]\n", -42);
		ft_printf("ft_printf:	[%-15.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][0][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+10.5d]\n", -42);
		ft_printf("ft_printf:	[%-+10.5d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-5.10d]\n", -42);
		ft_printf("ft_printf:	[%-5.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%d]\n", 42);
		ft_printf("ft_printf:	[%d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%d]\n", 0);
		ft_printf("ft_printf:	[%d]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%010d]\n", 0);
		ft_printf("ft_printf:	[%010d]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10d]\n", 42);
		ft_printf("ft_printf:	[%10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+10d]\n", 42);
		ft_printf("ft_printf:	[%+10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][ ][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+ 10d]\n", 42);
		ft_printf("ft_printf:	[%+ 10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.10][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.10d]\n", 42);
		ft_printf("ft_printf:	[%.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+ .10d]\n", 42);
		ft_printf("ft_printf:	[%+ .10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%5.10d]\n", 42);
		ft_printf("ft_printf:	[%5.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10d]\n", 42);
		ft_printf("ft_printf:	[%10.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [15][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%15.10d]\n", 42);
		ft_printf("ft_printf:	[%15.10d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][0][10][.5][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+010.5d]\n", 42);
		ft_printf("ft_printf:	[%+010.5d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][0][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.15d]\n", 42);
		ft_printf("ft_printf:	[%10.15d]\n", 42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%d]\n", -42);
		ft_printf("ft_printf:	[%d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10d]\n", -42);
		ft_printf("ft_printf:	[%10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+10d]\n", -42);
		ft_printf("ft_printf:	[%+10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][ ][10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+ 10d]\n", -42);
		ft_printf("ft_printf:	[%+ 10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.10][d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.10d]\n", -42);
		ft_printf("ft_printf:	[%.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][ ][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+ .10d]\n", -42);
		ft_printf("ft_printf:	[%+ .10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%5.10d]\n", -42);
		ft_printf("ft_printf:	[%5.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10d]\n", -42);
		ft_printf("ft_printf:	[%10.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [15][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%15.10d]\n", -42);
		ft_printf("ft_printf:	[%15.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][0][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+10.5d]\n", -42);
		ft_printf("ft_printf:	[%+010.5d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%5.10d]\n", -42);
		ft_printf("ft_printf:	[%5.10d]\n", -42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [5][.10][d]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%5.0d]\n", 0);
		ft_printf("ft_printf:	[%5.0d]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%hhd]\n", 21474836485);
		ft_printf("ft_printf:	[%hhd]\n", 21474836485);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [d]							--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%d]\n", -9223372036854775808);
		ft_printf("ft_printf:	[%d]\n", -9223372036854775808);
		check_equal(&stdout_save, less);
	}
	done++;
}

int glob = 42;
int *ptr1 = &glob;

void	unit_specifier_p(char less)
{
	static done = 0;

	if (!done)
	{
		int i = 1;
		int a = 42;
		int *ptr2 = &a;
		static int b = 42;
		static int *ptr3 = &b;
		void *ptr4 = (void *)42465;
		long long ll = (long long)__LONG_LONG_MAX__;
		long long *llp = &ll;
		
		printf("\n\033[1;30m----[Test for Specifier p]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (pointer to global variable (static memory))	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ptr1);
		ft_printf("ft_printf:	[%p]\n", ptr1);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (pointer to stack memory)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ptr2);
		ft_printf("ft_printf:	[%p]\n", ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (pointer to static variable)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ptr3);
		ft_printf("ft_printf:	[%p]\n", ptr3);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (pointer to manually assigned address)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ptr4);
		ft_printf("ft_printf:	[%p]\n", ptr4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ptr2);
		ft_printf("ft_printf:	[%p]\n", ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][p]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10p]\n", ptr2);
		ft_printf("ft_printf:	[%10p]\n", ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10][p] (Address longer then width)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10p]\n", ptr2);
		ft_printf("ft_printf:	[%10p]\n", ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][p]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20p]\n", ptr2);
		ft_printf("ft_printf:	[%-20p]\n", ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][p]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*p]\n", 20, ptr2);
		ft_printf("ft_printf:	[%*p]\n", 20, ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][p] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*p]\n", -20, ptr2);
		ft_printf("ft_printf:	[%*p]\n", -20, ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][p] (Address longer then width)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*p]\n", 10, ptr2);
		ft_printf("ft_printf:	[%-*p]\n", 10, ptr2);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (Pointer to long long)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", llp);
		ft_printf("ft_printf:	[%p]\n", llp);
		check_equal(&stdout_save, less);
		
		long double ld = __LDBL_MAX__;
		long double *ldp = &ld;
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [p] (Pointer to long double)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%p]\n", ldp);
		ft_printf("ft_printf:	[%p]\n", ldp);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_unsigned(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;

		printf("\n\033[1;30m----[Test for Specifier unsigned]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%u]\n", 142424242);
		ft_printf("ft_printf:	[%u]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [o]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%o]\n", 142424242);
		ft_printf("ft_printf:	[%o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%u]\n", 0);
		ft_printf("ft_printf	[%u]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [o]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%o]\n", 0);
		ft_printf("ft_printf	[%o]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%x]\n", 0);
		ft_printf("ft_printf	[%x]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [X]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%X]\n", 0);
		ft_printf("ft_printf	[%X]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%x]\n", 142424242);
		ft_printf("ft_printf:	[%x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [X]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%X]\n", 142424242);
		ft_printf("ft_printf:	[%X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][o]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#o]\n", 142424242);
		ft_printf("ft_printf:	[%#o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#x]\n", 142424242);
		ft_printf("ft_printf:	[%#x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][X]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#X]\n", 142424242);
		ft_printf("ft_printf:	[%#X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [20][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20u]\n", 142424242);
		ft_printf("ft_printf:	[%20u]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][20][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#20o]\n", 142424242);
		ft_printf("ft_printf:	[%#20o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][20][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#20x]\n", 142424242);
		ft_printf("ft_printf:	[%#20x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][20][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#20X]\n", 142424242);
		ft_printf("ft_printf:	[%#20X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][u]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20u]\n", 142424242);
		ft_printf("ft_printf:	[%-20u]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20o]\n", 142424242);
		ft_printf("ft_printf:	[%#-20o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20x]\n", 142424242);
		ft_printf("ft_printf:	[%#-20x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20X]\n", 142424242);
		ft_printf("ft_printf:	[%#-20X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][o]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*o]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#*o]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*x]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#*x]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][X]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*X]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#*X]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%-*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][u] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*u]\n", -20, 142424242);
		ft_printf("ft_printf:	[%*u]\n", -20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][u] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*u]\n", -20, 142424242);
		ft_printf("ft_printf:	[%*u]\n", -20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][u] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*u]\n", -20, 142424242);
		ft_printf("ft_printf:	[%*u]\n", -20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][u] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*u]\n", -20, 142424242);
		ft_printf("ft_printf:	[%*u]\n", -20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.*][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%.*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][.*][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#.*o]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#.*o]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][.*][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#.*x]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#.*x]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][.*][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#.*X]\n", 20, 142424242);
		ft_printf("ft_printf:	[%#.*X]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][.*][u]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*.*u]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%*.*u]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][o]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*o]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%#*.*o]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][x]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*x]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%#*.*x]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][X]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*X]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%#*.*X]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [*][.*][u]	(precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%*.*u]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%*.*u]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][o]	(precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*o]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%#*.*o]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][x]	(precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*x]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%#*.*x]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][*][.*][X]	(precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#*.*X]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%#*.*X]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X]	(width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][20][u]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%020u]\n", 142424242);
		ft_printf("ft_printf:	[%020u]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][20][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#20o]\n", 142424242);
		ft_printf("ft_printf:	[%0#20o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][20][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#20x]\n", 142424242);
		ft_printf("ft_printf:	[%0#20x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][20][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#20X]\n", 142424242);
		ft_printf("ft_printf:	[%0#20X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][u]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20u]\n", 142424242);
		ft_printf("ft_printf:	[%-20u]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20o]\n", 142424242);
		ft_printf("ft_printf:	[%#-20o]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20x]\n", 142424242);
		ft_printf("ft_printf:	[%#-20x]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][-][20][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#-20X]\n", 142424242);
		ft_printf("ft_printf:	[%#-20X]\n", 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][*][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*o]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#*o]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*x]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#*x]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*X]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#*X]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][u]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%-*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][*][u] (negative arg)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0*u]\n", -20, 142424242);
		ft_printf("ft_printf:	[%0*u]\n", -20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][.*][u]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0.*u]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0.*u]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][.*][o]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#.*o]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#.*o]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][.*][x]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#.*x]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#.*x]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][.*][X]					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#.*X]\n", 20, 142424242);
		ft_printf("ft_printf:	[%0#.*X]\n", 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][*][.*][u] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0*.*u]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%0*.*u]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][o] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*o]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%0#*.*o]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][x] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*x]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%0#*.*x]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][X] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*X]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%0#*.*X]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][*][.*][u] (precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0*.*u]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%0*.*u]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][o] (precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*o]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%0#*.*o]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][x] (precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*x]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%0#*.*x]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][#][*][.*][X] (precision bigger than width)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%0#*.*X]\n", 20, 40, 142424242);
		ft_printf("ft_printf:	[%0#*.*X]\n", 20, 40, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (width bigger than precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 40, 20, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 40, 20, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (small width and precision)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 5, 5, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 5, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 5, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 5, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 5, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 5, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 5, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 5, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (small width and precision)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 0, 5, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 0, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 0, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 0, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 0, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 0, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 0, 5, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 0, 5, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (small width and precision)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 5, 0, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 5, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 5, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 5, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 5, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 5, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 5, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 5, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (small width and precision)		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 0, 0, 142424242);
		ft_printf("ft_printf:	[%-*.*u]\n", 0, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 0, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*o]\n", 0, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 0, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*x]\n", 0, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (small width and precision)	--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 0, 0, 142424242);
		ft_printf("ft_printf:	[%-#*.*X]\n", 0, 0, 142424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 0, 0, 0);
		ft_printf("ft_printf:	[%-*.*u]\n", 0, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 0, 0, 0);
		ft_printf("ft_printf:	[%-#*.*o]\n", 0, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 0, 0, 0);
		ft_printf("ft_printf:	[%-#*.*x]\n", 0, 0, 0);
		check_equal(&stdout_save, less);
		//0x]
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 0, 0, 0);
		ft_printf("ft_printf:	[%-#*.*X]\n", 0, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 5, 0, 0);
		ft_printf("ft_printf:	[%-*.*u]\n", 5, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 5, 0, 0);
		ft_printf("ft_printf:	[%-#*.*o]\n", 5, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 5, 0, 0);
		ft_printf("ft_printf:	[%-#*.*x]\n", 5, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 5, 0, 0);
		ft_printf("ft_printf:	[%-#*.*X]\n", 5, 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 0, 5, 0);
		ft_printf("ft_printf:	[%-*.*u]\n", 0, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 0, 5, 0);
		ft_printf("ft_printf:	[%-#*.*o]\n", 0, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 0, 5, 0);
		ft_printf("ft_printf:	[%-#*.*x]\n", 0, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 0, 5, 0);
		ft_printf("ft_printf:	[%-#*.*X]\n", 0, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][*][.*][u] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-*.*u]\n", 5, 5, 0);
		ft_printf("ft_printf:	[%-*.*u]\n", 5, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][o] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*o]\n", 5, 5, 0);
		ft_printf("ft_printf:	[%-#*.*o]\n", 5, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][x] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*x]\n", 5, 5, 0);
		ft_printf("ft_printf:	[%-#*.*x]\n", 5, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][#][*][.*][X] (number = 0)			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-#*.*X]\n", 5, 5, 0);
		ft_printf("ft_printf:	[%-#*.*X]\n", 5, 5, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [llu]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%llu]\n", ULLONG_MAX);
		ft_printf("ft_printf:	[%llu]\n", ULLONG_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#][llX]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#llX]\n", ULLONG_MAX);
		ft_printf("ft_printf:	[%#llX]\n", ULLONG_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%x]\n", 4294967296);
		ft_printf("ft_printf:	[%x]\n", 4294967296);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [X]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%X]\n", 4294967296);
		ft_printf("ft_printf:	[%X]\n", 4294967296);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [hx]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%hx]\n", 4294967296);
		ft_printf("ft_printf:	[%hx]\n", 4294967296);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [hhx]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%hhx]\n", 4294967296);
		ft_printf("ft_printf:	[%hhx]\n", 4294967296);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [#x]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%#x]\n", 0);
		ft_printf("ft_printf:	[%#x]\n", 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.o]						--> ");
		stream_to_file(&stdout_save, buffer);
		ft_printf("ft_printf	[%.o %.o]\n", 0, 0);
		printf("printf		[%.o %.o]\n", 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.o]						--> ");
		stream_to_file(&stdout_save, buffer);
		ft_printf("ft_printf	[%#.o %#.o]\n", 0, 0);
		printf("printf		[%#.o %#.o]\n", 0, 0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.o]						--> ");
		stream_to_file(&stdout_save, buffer);
		ft_printf("ft_printf	[%#llx]\n", ULONG_MAX);
		printf("printf		[%#llx]\n", ULONG_MAX);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_color(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier color]----\033[0m\n");
		printf("----[Test %i]----\n", i++);
		printf("Specifier   = [color]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		["UNDERLINE BOLD ITALICS"tekst2 " UNDERLINE_OFF ITALICS_OFF MAGENTA " tekst3" EOC "]\n");
		ft_printf("ft_printf	[{underline}{bold}{italics}tekst2 {underline off}{italics off}{magenta} tekst3{eoc}]\n");
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [color]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		["YELLOW BOLD ITALICS"tekst2 %#x" UNDERLINE_OFF ITALICS_OFF BLUE " %f tekst3" EOC "]\n", 534, 324.235);
		ft_printf("ft_printf	[{yellow}{bold}{italics}tekst2 %#x{underline off}{italics off}{blue} %f tekst3{eoc}]\n", 534, 324.235);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [color]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[" GREEN BOLD "SUCCES" EOC BOLD"]\n", 534, 324.235);
		ft_printf("ft_printf	[{green}{bold}SUCCES{eoc}{bold}]\n", 534, 324.235);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [color]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[" RED BOLD "FAIL" EOC"]\n", 534, 324.235);
		ft_printf("ft_printf	[{red}{bold}FAIL{eoc}]\n", 534, 324.235);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_arg_index(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier arg_index]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [%%4$d, %%1$d, %%3$d, %%2$d]				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%4$d, %1$d, %3$d, %2$d]\n", 10, 100, 1000, 10000);
		ft_printf("ft_printf	[%4$d, %1$d, %3$d, %2$d]\n", 10, 100, 1000, 10000);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [%%4$#x, %%1$#x, %%3$#x, %%2$#x]			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%4$#x, %1$#x, %3$#x, %2$#x]\n", 42, 4242, 424242, 42424242);
		ft_printf("ft_printf	[%4$#x, %1$#x, %3$#x, %2$#x]\n", 42, 4242, 424242, 42424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [%%4$lf, %%1$c, %%3$s, %%2$hhi]			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%4$lf, %1$c, %3$s, %2$hhi]\n", 'N', (char)4242, "fortytwo", 42.42424242);
		ft_printf("ft_printf	[%4$lf, %1$c, %3$s, %2$hhi]\n", 'N', (char)4242, "fortytwo", 42.42424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [%%4$#2.4x, %%1$-8.5x, %%3$1.9x, %%2$#10.0x]		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%4$#2.4x, %1$-8.5x, %3$1.9x, %2$#10.0x]\n", 42, 4242, 424242, 42424242);
		ft_printf("ft_printf	[%4$#2.4x, %1$-8.5x, %3$1.9x, %2$#10.0x]\n", 42, 4242, 424242, 42424242);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [%%4$#2.4x, %%1$-8.5x, %%3$1.9x, %%2$#10.0x]		--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf		[%4$#X, %4$X, %4$x, %4$o]\n", 42, 4242, 424242, 42424242);
		ft_printf("ft_printf	[%4$#X, %4$X, %4$x, %4$o]\n", 42, 4242, 424242, 42424242);
		check_equal(&stdout_save, less);
	}
	done++;
}
//(nan)
void	unit_specifier_f(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		
		printf("\n\033[1;30m----[Test for Specifier f]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-f]\n", (double)42);
		ft_printf("ft_printf:	[%-f]\n", (double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-f]\n", (double)0);
		ft_printf("ft_printf:	[%-f]\n", (double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.0f]\n", (double)-9.4);
		ft_printf("ft_printf:	[%-.0f]\n", (double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test number 4{%i}]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0f]\n", (double)9.5);
		ft_printf("ft_printf:	[%- .0f]\n", (double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0f]\n", (double)-989.5);
		ft_printf("ft_printf:	[%- .0f]\n", (double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.50f]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%-+.50f]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test 100000000]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20.10f]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%-20.10f]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.60f]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%-+.60f]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%f]\n", (double)42);
		ft_printf("ft_printf:	[%f]\n", (double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%f]\n", (double)0);
		ft_printf("ft_printf:	[%f]\n", (double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.0f]\n", (double)-9.4);
		ft_printf("ft_printf:	[%.0f]\n", (double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0f]\n", (double)9.5);
		ft_printf("ft_printf:	[% .0f]\n", (double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0f]\n", (double)-989.5);
		ft_printf("ft_printf:	[% .0f]\n", (double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+.60f]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%+.60f]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.10f]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%20.10f]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%020.10f]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%020.10f]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test 500000]----\n", i++);
		printf("Specifier   = [f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+020.10f]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%+020.10f]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][f] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-f]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-f]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][f] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10f]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10f]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][f] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-f]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-f]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][f] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10f]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10f]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][f] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-f]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-f]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][f] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10f]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10f]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%f]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%f]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][f] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10f]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10f]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%f]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%f]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][f] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10f]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10f]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%f]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%f]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][f] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10f]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%10.10f]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][lf] (DBL_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500lf]\n", DBL_TRUE_MIN);
		ft_printf("ft_printf:	[%.500lf]\n", DBL_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [lf] (DBL_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%lf]\n", DBL_MAX);
		ft_printf("ft_printf:	[%lf]\n", DBL_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][lf] (DBL_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500lf]\n", DBL_MIN);
		ft_printf("ft_printf:	[%.500lf]\n", DBL_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][f] (FLT_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200f]\n", FLT_TRUE_MIN);
		ft_printf("ft_printf:	[%.200f]\n", FLT_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f] (FLT_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%lf]\n", FLT_MAX);
		ft_printf("ft_printf:	[%lf]\n", FLT_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][f] (FLT_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200lf]\n", FLT_MIN);
		ft_printf("ft_printf:	[%.200lf]\n", FLT_MIN);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_f_long(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier f long]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Lf]\n", (long double)42);
		ft_printf("ft_printf:	[%-Lf]\n", (long double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Lf]\n", (long double)0);
		ft_printf("ft_printf:	[%-Lf]\n", (long double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.0Lf]\n", (long double)-9.4);
		ft_printf("ft_printf:	[%-.0Lf]\n", (long double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][ ][.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0Lf]\n", (long double)9.5);
		ft_printf("ft_printf:	[%- .0Lf]\n", (long double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][ ][.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0Lf]\n", (long double)-989.5);
		ft_printf("ft_printf:	[%- .0Lf]\n", (long double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][.50][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.50Lf]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%-+.50Lf]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][.10][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20.10Lf]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%-20.10Lf]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][.60][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.60Lf]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%-+.60Lf]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", (long double)42);
		ft_printf("ft_printf:	[%Lf]\n", (long double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", (long double)0);
		ft_printf("ft_printf:	[%Lf]\n", (long double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.0Lf]\n", (long double)-9.4);
		ft_printf("ft_printf:	[%.0Lf]\n", (long double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [ ][.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0Lf]\n", (long double)9.5);
		ft_printf("ft_printf:	[% .0Lf]\n", (long double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [ ][.0][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0Lf]\n", (long double)-989.5);
		ft_printf("ft_printf:	[% .0Lf]\n", (long double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][.60][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+.60Lf]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%+.60Lf]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [20][.10][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.10Lf]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%20.10Lf]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][20][.10][L][f]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%020.10Lf]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%020.10Lf]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][f] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Lf]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-Lf]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][f] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Lf]\n", (long double)(-1.0 / 0.0));
		ft_printf("ft_printf:	[%-10.10Lf]\n", (long double)(-1.0 / 0.0));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][f] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Lf]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-Lf]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][f] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Lf]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10Lf]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][f] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Lf]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-Lf]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][f] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Lf]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10Lf]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%Lf]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][f] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Lf]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Lf]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%Lf]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][f] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Lf]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Lf]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%Lf]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][f] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Lf]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Lf]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][L][f] (LDBL_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500Lf]\n", LDBL_TRUE_MIN);
		ft_printf("ft_printf:	[%.500Lf]\n", LDBL_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (LDBL_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Lf]\n", LDBL_MAX);
		ft_printf("ft_printf:	[%Lf]\n", LDBL_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][L][f] (LDBL_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500Lf]\n", LDBL_MIN);
		ft_printf("ft_printf:	[%.500Lf]\n", LDBL_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][f] (FLT_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200f]\n", FLT_TRUE_MIN);
		ft_printf("ft_printf:	[%.200f]\n", FLT_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [f] (FLT_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%lf]\n", FLT_MAX);
		ft_printf("ft_printf:	[%lf]\n", FLT_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][f] (FLT_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200lf]\n", FLT_MIN);
		ft_printf("ft_printf:	[%.200lf]\n", FLT_MIN);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_e(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier e]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-e]\n", (double)42);
		ft_printf("ft_printf:	[%-e]\n", (double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-e]\n", (double)0);
		ft_printf("ft_printf:	[%-e]\n", (double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.0e]\n", (double)-9.4);
		ft_printf("ft_printf:	[%-.0e]\n", (double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0e]\n", (double)9.5);
		ft_printf("ft_printf:	[%- .0e]\n", (double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0e]\n", (double)-989.5);
		ft_printf("ft_printf:	[%- .0e]\n", (double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.50e]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%-+.50e]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20.10e]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%-20.10e]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.60e]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%-+.60e]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%e]\n", (double)42);
		ft_printf("ft_printf:	[%e]\n", (double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%e]\n", (double)0);
		ft_printf("ft_printf:	[%e]\n", (double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.0e]\n", (double)-9.4);
		ft_printf("ft_printf:	[%.0e]\n", (double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0e]\n", (double)9.5);
		ft_printf("ft_printf:	[% .0e]\n", (double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0e]\n", (double)-989.5);
		ft_printf("ft_printf:	[% .0e]\n", (double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+.60e]\n", (double)((double)10 / 3));
		ft_printf("ft_printf:	[%+.60e]\n", (double)((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.10e]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%20.10e]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%020.10e]\n", (double)-((double)10 / 3));
		ft_printf("ft_printf:	[%020.10e]\n", (double)-((double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][e] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-e]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-e]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][e] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10e]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10e]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][e] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-e]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-e]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][e] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10e]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10e]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][e] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-e]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-e]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][e] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10e]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10e]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][le] (DBL_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500le]\n", DBL_TRUE_MIN);
		ft_printf("ft_printf:	[%.500le]\n", DBL_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [le] (DBL_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%le]\n", DBL_MAX);
		ft_printf("ft_printf:	[%le]\n", DBL_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][le] (DBL_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500le]\n", DBL_MIN);
		ft_printf("ft_printf:	[%.500le]\n", DBL_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][e] (FLT_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200e]\n", FLT_TRUE_MIN);
		ft_printf("ft_printf:	[%.200e]\n", FLT_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e] (FLT_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%le]\n", FLT_MAX);
		ft_printf("ft_printf:	[%le]\n", FLT_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][e] (FLT_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200le]\n", FLT_MIN);
		ft_printf("ft_printf:	[%.200le]\n", FLT_MIN);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_specifier_e_long(char less)
{
	static done = 0;
	
	if (!done)
	{
		int i = 1;
		printf("\n\033[1;30m----[Test for Specifier e long]----\033[0m\n");
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Le]\n", (long double)42);
		ft_printf("ft_printf:	[%-Le]\n", (long double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Le]\n", (long double)0);
		ft_printf("ft_printf:	[%-Le]\n", (long double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-.0Le]\n", (long double)-9.4);
		ft_printf("ft_printf:	[%-.0Le]\n", (long double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][ ][.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0Le]\n", (long double)9.5);
		ft_printf("ft_printf:	[%- .0Le]\n", (long double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][ ][.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%- .0Le]\n", (long double)-989.5);
		ft_printf("ft_printf:	[%- .0Le]\n", (long double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][.50][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.50Le]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%-+.50Le]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][20][.10][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-20.10Le]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%-20.10Le]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][+][.60][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-+.60Le]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%-+.60Le]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", (long double)42);
		ft_printf("ft_printf:	[%Le]\n", (long double)42);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", (long double)0);
		ft_printf("ft_printf:	[%Le]\n", (long double)0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.0Le]\n", (long double)-9.4);
		ft_printf("ft_printf:	[%.0Le]\n", (long double)-9.4);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [ ][.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0Le]\n", (long double)9.5);
		ft_printf("ft_printf:	[% .0Le]\n", (long double)9.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [ ][.0][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[% .0Le]\n", (long double)-989.5);
		ft_printf("ft_printf:	[% .0Le]\n", (long double)-989.5);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [+][.60][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%+.60Le]\n", (long double)((long double)10 / 3));
		ft_printf("ft_printf:	[%+.60Le]\n", (long double)((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [20][.10][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%20.10Le]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%20.10Le]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [0][20][.10][L][e]						--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%020.10Le]\n", (long double)-((long double)10 / 3));
		ft_printf("ft_printf:	[%020.10Le]\n", (long double)-((long double)10 / 3));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][e] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Le]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%-Le]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][e] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Le]\n", (long double)(-1.0 / 0.0));
		ft_printf("ft_printf:	[%-10.10Le]\n", (long double)(-1.0 / 0.0));
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][e] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Le]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-Le]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][e] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Le]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10Le]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][L][e] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-Le]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-Le]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [-][10.10][L][e] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%-10.10Le]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%-10.10Le]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		//
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][e] (-inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%Le]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][e] (-inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Le]\n", -1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Le]\n", -1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][e] (inf)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%Le]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][e] (inf)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Le]\n", 1.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Le]\n", 1.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (nan)					--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%Le]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [10.10][L][e] (nan)				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%10.10Le]\n", 0.0 / 0.0);
		ft_printf("ft_printf:	[%10.10Le]\n", 0.0 / 0.0);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][L][e] (LDBL_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500Le]\n", LDBL_TRUE_MIN);
		ft_printf("ft_printf:	[%.500Le]\n", LDBL_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [L][f] (LDBL_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%Le]\n", LDBL_MAX);
		ft_printf("ft_printf:	[%Le]\n", LDBL_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.500][L][e] (LDBL_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.500Le]\n", LDBL_MIN);
		ft_printf("ft_printf:	[%.500Le]\n", LDBL_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][e] (FLT_TRUE_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200e]\n", FLT_TRUE_MIN);
		ft_printf("ft_printf:	[%.200e]\n", FLT_TRUE_MIN);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [e] (FLT_MAX (bonus))				--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%lf]\n", FLT_MAX);
		ft_printf("ft_printf:	[%lf]\n", FLT_MAX);
		check_equal(&stdout_save, less);
		
		if (!less)
			printf("----[Test %i]----\n", i++);
		printf("Specifier   = [.200][e] (FLT_MIN (bonus))			--> ");
		stream_to_file(&stdout_save, buffer);
		printf("printf:		[%.200le]\n", FLT_MIN);
		ft_printf("ft_printf:	[%.200le]\n", FLT_MIN);
		check_equal(&stdout_save, less);
	}
	done++;
}

void	unit_tests_42()
{
	static done = 0;
	
	if (!done)
	{
		int	ret;
		
		ret = printf("");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("1|\n|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("1|\n|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("2|test|");
		printf("\tft_printf ret = %d\n", ret);
		ret = ft_printf("2|test|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("3|test\n|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("3|test\n|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("4|1234|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("4|1234|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("5|%%|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("5|%%|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("6|%5%|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("6|%5%|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("7|%-5%|");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("7|%-5%|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = ft_printf("8|%.0%|");
		printf("\tft_printf ret = %d\n", ret);
		ret = ft_printf("8|%.0%|");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("9|%%|", "test");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("9|%%|", "test");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("10|%   %|", "test");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("10|%   %|", "test");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("11|%x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("11|%x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("12|%X|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("12|%X|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("13|%x|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("13|%x|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("14|%X|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("14|%X|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("15|%x|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("15|%x|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("16|%X|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("16|%X|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("17|%x|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("17|%x|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("18|%X|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("18|%X|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("19|%x|", "test");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("19|%x|", "test");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("20|%10x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("20|%10x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("21|%-10x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("21|%-10x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("22|%lx|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("22|%lx|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("23|%llX|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("23|%llX|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("24|%hx|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("24|%hx|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("25|%hhX|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("25|%hhX|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("25.1|%llx|", 9223372036854775807);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("25.1|%llx|", 9223372036854775807);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("26|%llx|", 9223372036854775808);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("26|%llx|", 9223372036854775808);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("27|%010x|", 542);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("27|%010x|", 542);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("28|%-15x|", 542);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("28|%-15x|", 542);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("29|%2x|", 542);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("29|%2x|", 542);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("30|%.2x|", 5427);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("30|%.2x|", 5427);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("31|%5.2x|", 5427);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("31|%5.2x|", 5427);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("32|%#x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("32|%#x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("33|%#llx|", 9223372036854775807);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("33|%#llx|", 9223372036854775807);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("34|%#x|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("34|%#x|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("35|%#x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("35|%#x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("36|%#X|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("36|%#X|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("37|%%#8x||%#8x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("37|%%#8x||%#8x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("38|%#08x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("38|%#08x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("39|%#-08x|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("39|%#-08x|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("40|@moulitest: %#.x %#.0x|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("40|@moulitest: %#.x %#.0x|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("41|@moulitest: |%.x| |%.0x|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("41|@moulitest: |%.x| |%.0x|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("42|@moulitest: %%5.x %%5.0x||@moulitest: |%5.x| |%5.0x|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("42|@moulitest: %%5.x %%5.0x||@moulitest: |%5.x| |%5.0x|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("43|%s|", "abc");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("43|%s|", "abc");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("44|%s|", "this is a string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("44|%s|", "this is a string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("45|%s |", "this is a string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("45|%s |", "this is a string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("46|%s  |", "this is a string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("46|%s  |", "this is a string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("47|this is a %s|", "string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("47|this is a %s|", "string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("48|%s is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("48|%s is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("49|Line Feed %s|", "\n");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("49|Line Feed %s|", "\n");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("50|%10s is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("50|%10s is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("51|%%.2s is a string||%.2s| is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("51|%%.2s is a string||%.2s| is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("52|%%5.2s is a string||%5.2s| is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("52|%%5.2s is a string||%5.2s| is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("53|%%10s| is a string||%10s| is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("53|%%10s| is a string||%10s| is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("54|%.2s| is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("54|%.2s| is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("55|%%5.2s is a string||%5.2s| is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("55|%%5.2s is a string||%5.2s| is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("56|%-10s| is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("56|%-10s| is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("57|%-.2s| is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("57|%-.2s| is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("58|%-5.2s| is a string|", "this");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("58|%-5.2s| is a string|", "this");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("59|%-10s| is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("59|%-10s| is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("60|%-.2s| is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("60|%-.2s| is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("61|%-5.2s is a string|", "");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("61|%-5.2s is a string|", "");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("62|%s %s", "this|", "is");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("62|%s %s", "this|", "is");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("63|%s %s %s", "this|", "is", "a");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("63|%s %s %s", "this|", "is", "a");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("64|%s %s %s %s|", "this", "is", "a", "multi");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("64|%s %s %s %s|", "this", "is", "a", "multi");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("65|%s %s %s %s string. gg!|", "this", "is", "a", "multi", "string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("65|%s %s %s %s string. gg!|", "this", "is", "a", "multi", "string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("66|%s%s%s%s%s|", "this", "is", "a", "multi", "string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("66|%s%s%s%s%s|", "this", "is", "a", "multi", "string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("67|@moulitest: %s|", NULL);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("67|@moulitest: %s|", NULL);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("68|%.2c|", NULL);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("68|%.2c|", NULL);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("69|%s %s|", NULL, "string");
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("69|%s %s|", NULL, "string");
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("70|%c|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("70|%c|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("71|%5c|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("71|%5c|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("72|%-5c|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("72|%-5c|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("73|@moulitest: %c|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("73|@moulitest: %c|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("74|%2c|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("74|%2c|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("75|null %c and text|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("75|null %c and text|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("76|% c|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("76|% c|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("77|%o|", 40);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("77|%o|", 40);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("78|%5o|", 41);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("78|%5o|", 41);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("79|%05o|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("79|%05o|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("80|%-5o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("80|%-5o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("81|%#6o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("81|%#6o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("82|%-#6o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("82|%-#6o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("83|%-05o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("83|%-05o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("84|%-5.10o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("84|%-5.10o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("85|%-10.5o|", 2500);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("85|%-10.5o|", 2500);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("86|@moulitest: %o|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("86|@moulitest: %o|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("87|@moulitest: %.o %.0o|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("87|@moulitest: %.o %.0o|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("88|@moulitest: %5.o %5.0o|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("88|@moulitest: %5.o %5.0o|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("89|@moulitest: %#.o %#.0o|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("89|@moulitest: %#.o %#.0o|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("90|@moulitest: %.10o|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("90|@moulitest: %.10o|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("91|%d|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("91|%d|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("92|the %d|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("92|the %d|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("93|%d is one|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("93|%d is one|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("94|%d|", -1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("94|%d|", -1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("95|%d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("95|%d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("96|%d|", -4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("96|%d|", -4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("97|%d|", 2147483647);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("97|%d|", 2147483647);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("98|%d|", 2147483648);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("98|%d|", 2147483648);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("99|%d|", -2147483648);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("99|%d|", -2147483648);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("100|%d|", -2147483649);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("100|%d|", -2147483649);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("101|% d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("101|% d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("102|% d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("102|% d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("103|%+d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("103|%+d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("104|%+d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("104|%+d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("105|%+d|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("105|%+d|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		// ret = printf("106|%+d|", 242424242424242424242);
		// printf("\tprintf ret = %d\n", ret);
		// ret = ft_printf("106|%+d|", 242424242424242424242);
		// printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("107|% +d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("107|% +d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("108|% +d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("108|% +d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("109|%+ d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("109|%+ d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("110|%+ d|", -42);					//not working
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("110|%+ d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("111|%  +d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("111|%  +d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("112|%  +d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("112|%  +d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("113|%+  d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("113|%+  d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("114|%+  d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("114|%+  d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("115|% ++d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("115|% ++d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("116|% ++d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("116|% ++d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("117|%++ d|", 42);				//not working
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("117|%++ d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("118|%++ d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("118|%++ d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("119|%0d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("119|%0d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("120|%00d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("120|%00d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("121|%5d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("121|%5d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("122|%05d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("122|%05d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("123|%0+5d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("123|%0+5d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("124|%5d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("124|%5d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("125|%05d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("125|%05d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("126|%0+5d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("126|%0+5d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("127|%-5d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("127|%-5d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("128|%-05d", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("128|%-05d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("129|%-5d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("129|%-5d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("130|%-05d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("130|%-05d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("131|%hd|", 32767);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("131|%hd|", 32767);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("132|%hd|", -32768);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("132|%hd|", -32768);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("133|%hd|", 32768);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("133|%hd|", 32768);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("134|%hd|", -32769);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("134|%hd|", -32769);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("135|%hhd|", 127);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("135|%hhd|", 127);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("136|%hhd|", 128);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("136|%hhd|", 128);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("137|%hhd|", -128);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("137|%hhd|", -128);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("138|%hhd|", -129);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("138|%hhd|", -129);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("139|%ld|", 2147483647);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("139|%ld|", 2147483647);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("140|%ld|", -2147483648);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("140|%ld|", -2147483648);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("141|%ld|", 2147483648);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("141|%ld|", 2147483648);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("142|%ld|", -2147483649);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("142|%ld|", -2147483649);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("143|%lld|", 9223372036854775807);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("143|%lld|", 9223372036854775807);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("144|%lld|", -9223372036854775808);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("144|%lld|", -9223372036854775808);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("145|%d|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("145|%d|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("146|%d %d|", 1, -2);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("146|%d %d|", 1, -2);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("147|%d %d %d|", 1, -2, 33);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("147|%d %d %d|", 1, -2, 33);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("148|%d %d %d %d|", 1, -2, 33, 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("148|%d %d %d %d|", 1, -2, 33, 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("149|%d %d %d %d gg!|", 1, -2, 33, 42, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("149|%d %d %d %d gg!|", 1, -2, 33, 42, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("150|%4.15d|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("150|%4.15d|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("151|%.2d|", 4242);
		printf("\tprintf ret = %d\n\n", ret);
		ret = ft_printf("151|%.2d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("152|%.10d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("152|%.10d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("153|%10.5d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("153|%10.5d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("154|%-10.5d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("154|%-10.5d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("155|% 10.5d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("155|% 10.5d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("156|%+10.5d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("156|%+10.5d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("157|%-+10.5d|", 4242);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("157|%-+10.5d|", 4242);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("158|%03.2d|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("158|%03.2d|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("159|%03.2d|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("159|%03.2d|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("160|%03.2d|", -1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("160|%03.2d|", -1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("161|@moulitest: %.10d|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("161|@moulitest: %.10d|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("162|@moulitest: %.d %.0d|", 42, 43);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("162|@moulitest: %.d %.0d|", 42, 43);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("163|@moulitest: %.d %.0d|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("163|@moulitest: %.d %.0d|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("164|@moulitest: %5.d %5.0d|", 0, 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("164|@moulitest: %5.d %5.0d|", 0, 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("165|%u|", 0);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("165|%u|", 0);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("166|%u|", 1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("166|%u|", 1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("167|%u|", -1);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("167|%u|", -1);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("168|%u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("168|%u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("169|%u|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("169|%u|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("170|%5u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("170|%5u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("171|%15u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("171|%15u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("172|%-15u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("172|%-15u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("173|%015u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("173|%015u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("174|% u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("174|% u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("175|%+u|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("175|%+u|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("176|%lu|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("176|%lu|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("177|%lu|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("177|%lu|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("178|%lu|", -42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("178|%lu|", -42);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("179|%llu|", 4999999999);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("179|%llu|", 4999999999);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("180|%U|", 4294967295);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("180|%U|", 4294967295);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("181|%hU|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("181|%hU|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("182|%U|", 4294967296);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("182|%U|", 4294967296);
		printf("\tft_printf ret = %d\n\n", ret);
		
		ret = printf("183|@moulitest: %.5u|", 42);
		printf("\tprintf ret = %d\n", ret);
		ret = ft_printf("183|@moulitest: %.5u|", 42);
		printf("\tft_printf ret = %d\n\n", ret);
	}
	done++;
}

#include <unistd.h>

int     main(int argc, char **argv)
{
	char	i;
	char	less;

	if (argc < 2)
		exit(printf("usage: ./unit_test [less] all [c s d i p u color $ f e Lf Le 42]\n"));
	else
	{
		i = 1;
		if (!ft_strcmp(argv[i], "less\0"))
			less = 1;
		else
			less = 0;
		while (i < argc)
		{
			if (!ft_strcmp(argv[i], "all\0"))
			{
			unit_specifier_c(less);
			unit_specifier_s(less);
			unit_specifier_di(less);
			unit_specifier_p(less);
			unit_specifier_unsigned(less);
			unit_specifier_color(less);
			unit_specifier_arg_index(less);
			unit_specifier_f(less);
			unit_specifier_e(less);
			unit_specifier_f_long(less);
			unit_specifier_e_long(less);
			if (!less)
				unit_tests_42();
			}
			if (!ft_strcmp(argv[i], "c\0"))
				unit_specifier_c(less);
			if (!ft_strcmp(argv[i], "s\0"))
				unit_specifier_s(less);
			if (!ft_strcmp(argv[i], "d\0"))
				unit_specifier_di(less);
			if (!ft_strcmp(argv[i], "i\0"))
				unit_specifier_di(less);
			if (!ft_strcmp(argv[i], "p\0"))
				unit_specifier_p(less);
			if (!ft_strcmp(argv[i], "u\0"))
				unit_specifier_unsigned(less);
			if (!ft_strcmp(argv[i], "color\0"))
				unit_specifier_color(less);
			if (!ft_strcmp(argv[i], "$\0"))
				unit_specifier_arg_index(less);
			if (!ft_strcmp(argv[i], "f\0"))
				unit_specifier_f(less);
			if (!ft_strcmp(argv[i], "e\0"))
				unit_specifier_e(less);
			if (!ft_strcmp(argv[i], "Lf\0"))
				unit_specifier_f_long(less);
			if (!ft_strcmp(argv[i], "Le\0"))
				unit_specifier_e_long(less);
			if (!ft_strcmp(argv[i], "42\0") && !less)
				unit_tests_42();
			i++;
		}
	}
    return (0);
}
