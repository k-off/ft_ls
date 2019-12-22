# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: mgross <mgross@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2019/04/24 16:32:25 by mgross         #+#    #+#                 #
#    Updated: 2019/06/17 18:25:51 by nstabel       ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

all:
	@gcc -g -w -o unit_test unit_test.c -I. ../libftprintf.a

	@echo "usage: ./unit_test [less] all [flags] \n\
	flags:	c	- character \n\
		s	- string \n\
		p	- pointer \n\
		d / i	- integer \n\
		u	- unsigned (including hexadecimal and octal) \n\
		f	- floats \n\
		Lf	- long doubles \n\
		e	-   \n\
		Le	-   "