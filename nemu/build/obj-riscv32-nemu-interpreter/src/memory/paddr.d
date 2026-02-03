cmd_/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o := unused

source_/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o := src/memory/paddr.c

deps_/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o := \
    $(wildcard include/config/pmem/malloc.h) \
    $(wildcard include/config/pmem/garray.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/mem/random.h) \
    $(wildcard include/config/device.h) \
  /home/wiki/ysyx-workbench/nemu/include/memory/host.h \
    $(wildcard include/config/isa64.h) \
    $(wildcard include/config/rt/check.h) \
  /home/wiki/ysyx-workbench/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
  /home/wiki/ysyx-workbench/nemu/include/macro.h \
  /home/wiki/ysyx-workbench/nemu/include/debug.h \
  /home/wiki/ysyx-workbench/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  /home/wiki/ysyx-workbench/nemu/include/memory/paddr.h \
    $(wildcard include/config/pc/reset/offset.h) \
  /home/wiki/ysyx-workbench/nemu/include/device/mmio.h \
  /home/wiki/ysyx-workbench/nemu/include/isa.h \
  /home/wiki/ysyx-workbench/nemu/src/isa/riscv32/include/isa-def.h \
    $(wildcard include/config/rve.h) \
    $(wildcard include/config/rv64.h) \

/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o: $(deps_/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o)

$(deps_/home/wiki/ysyx-workbench/nemu/build/obj-riscv32-nemu-interpreter/src/memory/paddr.o):
