# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wchae <wchae@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/11 17:09:27 by wchae             #+#    #+#              #
#    Updated: 2022/06/12 15:21:17 by wchae            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= minishell

CC			= gcc
CFLAGS		= -Wall -Werror -Wextra

INCS		= -Ilibft -I.

SRCS_DIR		= srcs
SRCS_NAME		= main.c\
					handler.c

SRCS			= $(addprefix $(SRCS_DIR)/, $(SRCS_NAME))

OBJS_DIR		= objs
OBJS			= $(addprefix $(OBJS_DIR)/, $(SRCS_NAME:.c=.o))

LIB_DIR			= libft
LIB_NAME		= ft
LIB				= $(addprefix $(LIB_DIR)/, libft.a)

$(NAME)			: $(LIB) $(OBJS)
					$(CC) $(CFLAGS) $(INCS) -L$(LIB_DIR) -l$(LIB_NAME) -lreadline $(LDFLAGS) $(CPPFLAGS) -lncurses $(OBJS) -o $@

$(LIB)			:
					$(MAKE) -C $(LIB_DIR) all
					# $(MAKE) -C $(LIB_DIR) bonus

$(OBJS_DIR)/%.o	: $(SRCS_DIR)/%.c
					mkdir -p $(OBJS_DIR)
					$(CC) $(CFLAGS) $(INCS) $(CPPFLAGS) -c $< -o $@

all				: $(NAME)

clean			:
					$(MAKE) -C $(LIB_DIR) clean
					rm -rf $(OBJS_DIR)

fclean			:
					$(MAKE) -C $(LIB_DIR) fclean
					rm -rf $(NAME) $(OBJS_DIR)
re :
	make fclean
	make all

.PHONY: all clean fclean re bonus