CC := gcc
CFLAGS:=-Wall -g -m64
SRC:= $(wildcard *.c)
Tartget = pie_interface
.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(Tartget)

clean:
	rm -f $(Tartget)
