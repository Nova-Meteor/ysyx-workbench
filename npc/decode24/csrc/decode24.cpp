#include "Vdecode24.h"
#include "verilated.h"
#include <nvboard.h>

static TOP_NAME dut;

void nvboard_bind_all_pins(TOP_NAME* top);

int main(int argc, char** argv) {
  VerilatedContext* contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  TOP_NAME* top = new TOP_NAME{contextp};

  nvboard_bind_all_pins(&dut);
  nvboard_init();


  while (!contextp->gotFinish()) {
    nvboard_update();
    top->eval();
  }
  nvboard_quit();
  delete top;
  delete contextp;
  return 0;
}
