TOPNAME = switch
VERILATOR = verilator

VERILATOR_FLAGS = -Wall --cc --exe --build -j 0 --trace-fst

VSRCS = $(shell find ./vsrc -name "*.v")
CSRCS = $(shell find ./csrc -name "*.cpp")

all:
	@echo "Please specify a target (sim/wave/clean)"

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "--- VERILATOR BUILDING ---"
	$(VERILATOR) $(VERILATOR_FLAGS) $(VSRCS) $(CSRCS)
	@echo "--- SIMULATION RUNNING ---"
	./obj_dir/V$(TOPNAME)

wave:
	gtkwave simx.fst

clean:
	rm -rf obj_dir *.fst

include ../Makefile
