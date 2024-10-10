MAKEFLAGS += --no-print-directory

NAME = ft_ping

CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRCS = main.c packet.c
OBJS = $(SRCS:.c=.o)

all: $(NAME) animate

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Animation frames
FRAMES := frame1 frame2 frame3 frame4

# Default target
animate:
	@for frame in $(FRAMES); do \
		clear; \
		$(MAKE) $$frame; \
		sleep 0.5; \
	done

# Frame definitions
frame1:
	@echo "  O"
	@echo " /"
	@echo "Ping..."

frame2:
	@echo "   O"
	@echo "  /"
	@echo " Ping..."

frame3:
	@echo "    O"
	@echo "   /"
	@echo "  Ping..."

frame4:
	@echo "     O"
	@echo "    /"
	@echo "   Pong!"

.PHONY: all clean fclean re animate $(FRAMES)