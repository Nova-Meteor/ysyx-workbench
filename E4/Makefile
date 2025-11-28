.SUFFIXES:

CC	:=gcc
CFLAGS	:=-Wall
LDFLAGS	:=
RM	:=rm -f

all:ex1

.PHONY:all clean

ex1:ex1.o
	$(CC) $(LDFLAGS) -o $@ $<
	@echo ">>> Build success: $@"

DEPFLAGS=-MMD -MP
%.o:%.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include ex1.d

clean:
	$(RM) ex1 ex1.o ex1.d
