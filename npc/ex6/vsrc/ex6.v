module ex6(
    input btn_clk,        // 按钮作为时钟信号
    input btn_reset,      // 按钮作为复位信号
    output [7:0] seg1,    // 数码管1 (显示高4位)
    output [7:0] seg0     // 数码管0 (显示低4位)
  );

  wire [7:0] random_val;

  lfsr my_lfsr(
         .clk(btn_clk),
         .reset(btn_reset),
         .q(random_val)
       );

  bcd7seg seg_high(
            .b(random_val[7:4]),
            .h(seg1)
          );

  bcd7seg seg_low(
            .b(random_val[3:0]),
            .h(seg0)
          );

endmodule
