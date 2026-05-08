HEADERS = src/codexion.h

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = src/main.c src/args.c src/dongle.c src/error_message.c src/heap_utils.c src/heap.c src/init.c \
	src/input_utils.c src/monitor.c src/simulation_utils.c src/simulation.c src/utils_0.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

src/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re