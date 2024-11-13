MAKEFLAGS += --no-print-directory

NAME = ft_ping

CC = gcc
CFLAGS = -Wall -Wextra -Werror #-std=gnu90

SRCS = main.c ping.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re