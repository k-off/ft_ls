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
SRCS = *.c
MKOBJ = @gcc -c
FLAGS = -Wall -Wextra -Werror
HDRS = -I . ./Libft/*.h
OBJS = *.o
LFT = Libft/libft.a

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