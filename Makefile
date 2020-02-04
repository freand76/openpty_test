CFLAGS += -O2 -g

.PHONY: all clean

all: openpty

openpty: openpty.c
	gcc $^ -lutil -o $@

clean:
	rm -rf openpty
