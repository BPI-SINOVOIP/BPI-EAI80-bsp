SRCOBJS := $(patsubst %.c, %.o, $(wildcard *.c))

OBJS := $(SRCOBJS)
CC := gcc
BIN_TARGET := imageCompressTool
CFLAGS := -g

all:$(OBJS)
	@$(CC) -o $(BIN_TARGET) $(OBJS) $(LDFLAGS)
	
%.o: %.c
	@$(CC) -c $< $(CFLAGS) -o $@

clean:
	@rm	$(OBJS)
	@rm  $(BIN_TARGET)
