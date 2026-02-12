##The compiler to use
CC = gcc

## Compiler flags
CFLAGS = -static -Wall -Wextra -Werror -pedantic -O3

## Linking flags
LDFLAGS = $(CFLAGS)

## The name of the binary (executable) file - do not change
TARGET ?= alphachess

## Build the target by default
all: $(TARGET)

SRC = main.c ResetBoard.c PlayMove.c FEN.c showboard.c bestmove.c
OBJ = $(SRC:.c=.o)

$(TARGET) : $(OBJ)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)