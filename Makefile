TARGET = manul

CC = clang
LD = clang

LIBS =
CFLAGS = -emit-llvm -std=c99 -Wall -pedantic -Wno-unused-variable
LDFLAGS =

SOURCES := $(shell find . -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))

all: $(TARGET)

$(TARGET): ${OBJS}
	$(LD) ${OBJS} ${LIBS} ${LDFLAGS} -o ./"$(TARGET)"

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f ./$(TARGET)
	rm -f $(OBJS)
