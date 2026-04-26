HEADERS = codexion.h args.h error_message.h input_utils.h utils_0.h \
	heap_utils.h heap.h dongle.h init.h simulation_utils.h \
	simulation.h monitor.h

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = main.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

main.o: main.c $(HEADERS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re