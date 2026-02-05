#include "Vlight.h"
#include "verilated.h"
// #include "verilated_fst_c.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
#include <nvboard.h>

static TOP_NAME top;

void nvboard_bind_all_pins(Vlight* top);

static void single_cycle() {
  top.clk = 0; top.eval();
  top.clk = 1; top.eval();
}

static void reset(int n) {
  top.rst = 1;
  while (n -- > 0) single_cycle();
  top.rst = 0;
}

int main(int argc, char** argv) {
  VerilatedContext* contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  Vlight* top = new Vlight{contextp};

  // Verilated::traceEverOn(true);
  // VerilatedFstC* tfp = new VerilatedFstC;
  // top->trace(tfp, 99);
  // tfp->open("./obj_dir/simx.fst");

  nvboard_bind_all_pins(top);
  nvboard_init();

  reset(10);

  // while (contextp->time() < 20 && !contextp->gotFinish()) {
  while (!contextp->gotFinish()) {
    // int a = rand() & 1;
    // int b = rand() & 1;
    // top->a = a;
    // top->b = b;
    // top->eval();
    // tfp->dump(contextp->time());
    // contextp->timeInc(1);
    // printf("a = %d, b = %d, f = %d\n", a, b, top->f);
    // assert(top->f == (a ^ b));
    nvboard_update();
    single_cycle();
  }
  // tfp->close();
  nvboard_quit();
  delete top;
  delete contextp;
  return 0;
}
