NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread -I includes

SRCS        = src/main.c \
              src/main_init.c \
              src/main_utils.c \
              src/utils.c \
              concurrency/coder.c \
              concurrency/coder_phases.c \
              concurrency/coder_utils.c \
              concurrency/monitor.c \
              concurrency/dongle.c \
              concurrency/heap.c \
              concurrency/heap_utils.c

OBJS        = $(SRCS:.c=.o)

HEADER      = includes/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
