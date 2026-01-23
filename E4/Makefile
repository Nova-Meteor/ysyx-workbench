CC       := gcc
CFLAGS   := -Wall -O2 -g
DEPFLAGS := -MMD -MP
LDFLAGS  :=
RM       := rm -rf

SRCDIR := src
BLDDIR := build

# --------------- 用户可改区域 -----------------
# 1. 要生成的可执行文件（去掉 .c 后缀）
EXES   := ex31
# 2. 被多个 exe 引用的“公共”源文件（不带 main）
LIBS   := ex31.c
# --------------------------------------------

LIB_OBJS := $(patsubst %.c,$(BLDDIR)/%.o,$(LIBS))
EXE_OBJS := $(foreach e,$(EXES),$(BLDDIR)/$(e).o)
ALL_TGTS := $(foreach e,$(EXES),$(BLDDIR)/$(e))

all: $(ALL_TGTS)

# 链接规则：exe 需要自己的 .o + 所有 LIB_OBJS
$(BLDDIR)/%: $(BLDDIR)/%.o $(LIB_OBJS)
	@mkdir -p $(BLDDIR)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo ">>>  Build success: $@"

# 编译规则
$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BLDDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	$(RM) $(BLDDIR)

.PHONY: all clean

# 自动依赖
-include $(EXE_OBJS:.o=.d) $(LIB_OBJS:.o=.d)
