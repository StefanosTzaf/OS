CC = gcc

CFLAGS = -Wall -I ./include
OBJS = List.o
EXEC = lexan

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)
