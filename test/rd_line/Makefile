SRCS_DIR			= src

READLINE_DI			= readline
READLINE_INCLUDE	=	$(READLINE_DIR)
READLINE_LIB		=	$(READLINE_DIR)
READLINE			=	$(READLINE_DIR)/libreadline.a

NAME				= rdtest
CC					= cc
CFLAGS				= -Wall -Wextra -Werror -D READLINE_LIBARARY
SRCS				= $(SRCS_DIR)/main.c
OBJS				= $(SRCS:.c=.o)
RM					= rm
RMFLAGS				= -f

.PHONY				: all
all 				: $(NAME)

$(NAME)				:	$(OBJS) $(READLINE)
	$(CC) $(CFLAGS) -o $@ -L $(READLINE_LIB) lreadline -lhistory -lncurses

$(READLINE)			:
	cd $(READLINE_DIR); ./configure
	make -C $(READLINE_DIR)	all

$(LIBFT)			:
	make -C $(LIBFT_DIR) all

.PHONY				:	.c.o

.c.o				:
	$(CC) $(CFLAGS) -I$(READLINE_INCLUDE) -c $< -o $@

.PHONY				:	clean
clean				:	
	$(RM) $(RMFLAGS) $(OBJS)
	cd $(LIBFT_DIR); make clean

.PHONY				:	fclean
fclean				:	clean
	$(RM) $(RMFLAGS) $(NAME) $(LIBFT)
	make -C $(READLINE_DIR) clean

.PHONY				:	re
re					:	fclean all

.PHONY				:	bonus
bonus				:	all