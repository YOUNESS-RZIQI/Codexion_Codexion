NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread
SRCS    = coders/main.c coders/utils.c coders/dongle.c \
          coders/coder.c coders/monitor.c
OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -I coders/ -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
