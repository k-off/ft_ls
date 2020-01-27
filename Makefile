#******************************************************************************#
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: pacovali@codam.nl                            +#+                      #
#                                                    +#+                       #
#    Created: 2020/01/01 00:00:42 by pacovali      #+#    #+#                  #
#    Updated: 2020/01/01 00:00:42 by pacovali      ########   odam.nl          #
#                                                                              #
#******************************************************************************#

NAME = ft_ls

SRCS = main.c get_args.c get_info.c set_print.c sort_info.c print_info.c
OBJS = main.o get_args.o get_info.o set_print.o sort_info.o print_info.o

HDRS = -I . ./Libft/libft.h
LFT = Libft/libft.a

MKOBJ = @gcc -c
FLAGS = -Wall -Wextra -Werror
MKEXE = gcc -o $(NAME)

all: $(NAME)
$(NAME):
	@make -C Libft
	@$(MKOBJ) $(FLAGS) $(HDRS) $(SRCS)
	@$(MKEXE) $(OBJS) $(LFT)

clean:
	@make -C Libft clean
	@rm -f $(OBJS) *~

fclean: clean
	@make -C Libft fclean
	@rm -f $(NAME)

re: fclean all
