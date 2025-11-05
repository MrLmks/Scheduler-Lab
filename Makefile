
CC =	gcc
CFLAGS =	-Wall -Wextra -Werror
TARGET =	scheduler

SRCS =	src/scheduler_lab.c

OBJS =	$(SRCS:.c=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(TARGET) *.exe

re:
	fclean all