CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lX11 -lXext -lXrandr
SRC = src/snap_it.c src/utilities.c
OBJ = $(SRC:.c=.o)
EXEC = snap-it

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
