CC = gcc

CFLAGS = -Wall -I ./include

# Object files for root and splitter
ROOT_OBJS = List.o Map.o root.o
SPLITTER_OBJS = List.o Map.o splitter.o

# Executable names
ROOT_EXEC = lexan
SPLITTER_EXEC = splitter

# Default target: και τα 2 προγραμματα
all: $(ROOT_EXEC) $(SPLITTER_EXEC)

# Rule to create the root executable
$(ROOT_EXEC): $(ROOT_OBJS)
	$(CC) $(CFLAGS) -o $(ROOT_EXEC) $(ROOT_OBJS)

# Rule to create the splitter executable
$(SPLITTER_EXEC): $(SPLITTER_OBJS)
	$(CC) $(CFLAGS) -o $(SPLITTER_EXEC) $(SPLITTER_OBJS)

# Clean up object files and executables
clean:
	rm -f $(ROOT_OBJS) $(SPLITTER_OBJS) $(ROOT_EXEC) $(SPLITTER_EXEC)

clean_pipes:
	rm -f fifo*
