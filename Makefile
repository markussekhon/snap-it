CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/snap_it.c
OBJ = $(SRC:.c=.o)
EXEC = snap-it

all: $(EXEC)

$(EXEC): $(OBJ)
    $(CC) -o $@ $^

clean:
    rm -f $(OBJ) $(EXEC)

.PHONY: all clean

