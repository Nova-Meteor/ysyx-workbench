CC       := gcc
CFLAGS   := -Wall -O2 -g
DEPFLAGS := -MMD -MP
LDFLAGS  :=
RM       := rm -rf

SRCDIR := src
BLDDIR := build
SRCS   := $(wildcard $(SRCDIR)/*.c)
OBJS   := $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))
TGTS   := $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%,$(SRCS))

all: $(TGTS)

$(BLDDIR)/%: $(BLDDIR)/%.o
	@mkdir -p $(BLDDIR)
	$(CC) $(LDFLAGS) -o $@ $<
	@echo ">>>  Build success: $@"

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BLDDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(SRCS:$(SRCDIR)/%.c=$(BLDDIR)/%.d)

clean:
	$(RM) $(BLDDIR)

.PHONY: all clean
