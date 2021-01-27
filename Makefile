CC := gcc
CFLAGS:=-Wall -g -m64 -static
SRC:= $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRC)) 
EXT_OBJS := $(wildcard ../cJSON/*.o)
Tartget = pie_interface
.PHONY: all clean

all: $(OBJS) $(EXT_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(EXT_OBJS) -o $(Tartget)

$(OBJS): $(SRC)
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -f $(Tartget) *.o
