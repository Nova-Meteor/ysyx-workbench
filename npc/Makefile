TOPNAME = switch
NXDC_FILES = $(TOPNAME)/constr/switch.nxdc
BUILD_DIR = $(TOPNAME)/build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc \
                -O3 --x-assign fast --x-initial fast --noassert

include $(NVBOARD_HOME)/scripts/nvboard.mk

INC_PATH += $(abspath ./$(TOPNAME)/inc)
VSRCS = $(shell find $(abspath ./$(TOPNAME)/vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./$(TOPNAME)/csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	@mkdir -p $(BUILD_DIR)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

CSRCS += $(SRC_AUTO_BIND)

INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

all: sim

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

sim: $(BIN)
	$(call git_commit, "sim RTL") # 一生一芯进度追踪，请勿删除
	@echo "--- RUNNING NVBOARD ---"
	$(BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all sim clean
include ../Makefile
