CC = gcc

CFLAGS = -Wall -I ./include

# Object files for root and splitter
ROOT_OBJS = src/List.o src/Map.o src/root.o
SPLITTER_OBJS = src/List.o src/Map.o src/splitter.o
BUILDER_OBJS = src/List.o src/Map.o src/builder.o

# Executable names
ROOT_EXEC = lexan
SPLITTER_EXEC = splitter
BUILDER_EXEC = builder

# Default target: και τα 2 προγραμματα
all: $(ROOT_EXEC) $(SPLITTER_EXEC) $(BUILDER_EXEC)
	rm -f fifo*
	
# Rule to create the root executable
$(ROOT_EXEC): $(ROOT_OBJS)
	$(CC) $(CFLAGS) -o $(ROOT_EXEC) $(ROOT_OBJS)

# Rule to create the splitter executable
$(SPLITTER_EXEC): $(SPLITTER_OBJS)
	$(CC) $(CFLAGS) -o $(SPLITTER_EXEC) $(SPLITTER_OBJS)

$(BUILDER_EXEC): $(BUILDER_OBJS)
	$(CC) $(CFLAGS) -o $(BUILDER_EXEC) $(BUILDER_OBJS)

# Clean up object files and executables
clean:
	rm -f $(ROOT_OBJS) $(SPLITTER_OBJS) $(ROOT_EXEC) $(SPLITTER_EXEC) $(BUILDER_OBJS) $(BUILDER_EXEC)
	rm -f fifo*
