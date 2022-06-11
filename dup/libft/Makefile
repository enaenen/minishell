# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wchae <wchae@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/27 06:38:51 by wchae             #+#    #+#              #
#    Updated: 2022/06/12 18:12:51 by wchae            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= libft.a
CC			= gcc
CFLAGS			= -Wall -Wextra -Werror
RM			= rm -f
.SUFFIXES		= .c .o

SRCS			= ft_putstr_fd.c\
				ft_strlen.c\
				ft_split.c\
				ft_strtrim.c\
				ft_substr.c\
				ft_strjoin.c\
				ft_memset.c\
				ft_free.c\

OBJS			= $(SRCS:.c=.o)

BONUS			=	ft_lstnew.c\
				ft_lstsize.c\
				ft_lstlast.c\
				ft_lstadd_front.c\
				ft_lstadd_back.c\
				ft_lstdelone.c\
				ft_lstclear.c\
				ft_lstiter.c\
				ft_lstmap.c

BONUS_OBJS		= $(BONUS:.c=.o)


ifdef WITH_BONUS
	OBJ_FILES = $(OBJS) $(BONUS_OBJS)
else
	OBJ_FILES = $(OBJS)
endif

all:			$(NAME)

$(NAME):		$(OBJ_FILES)
				ar rc $(NAME) $(OBJ_FILES)

bonus: 
	make WITH_BONUS=1

clean:
				$(RM) $(OBJS) $(BONUS_OBJS)

fclean:			clean
				$(RM) $(NAME)

re:				
				make fclean 
				make all

.PHONY:			all clean fclean re bonus
