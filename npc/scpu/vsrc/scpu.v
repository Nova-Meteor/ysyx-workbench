module scpu(
    input clk,
    input rst,
    output [7:0] seg0, seg1,
    output [7:0] seg2, seg3, // r2 (debug)
    output [7:0] seg5        // PC (debug)
  );

  wire [7:0] display_val;
  wire [3:0] debug_pc;
  wire [7:0] debug_r2;

  cpu my_cpu(
        .clk(clk),
        .rst(rst),
        .out_val(display_val),
        .debug_pc(debug_pc),
        .debug_r2(debug_r2)
      );

  bcd7seg seg_low(
            .b(display_val[3:0]),
            .h(seg0)
          );
  bcd7seg seg_high(
            .b(display_val[7:4]),
            .h(seg1)
          );

  bcd7seg seg_r2_low(
            .b(debug_r2[3:0]),
            .h(seg2)
          );
  bcd7seg seg_r2_high(
            .b(debug_r2[7:4]),
            .h(seg3)
          );

  bcd7seg seg_pc_low(
            .b(debug_pc[3:0]),
            .h(seg5)
          );

endmodule
