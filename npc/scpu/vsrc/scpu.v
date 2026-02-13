module scpu(
    input clk,
    input rst,
    output [7:0] seg0,
    output [7:0] seg1
  );

  wire [7:0] display_val;

  cpu my_cpu(
        .clk(clk),
        .rst(rst),
        .out_val(display_val)
      );

  bcd7seg seg_low(
            .b(display_val[3:0]),
            .h(seg0)
          );
  bcd7seg seg_high(
            .b(display_val[7:4]),
            .h(seg1)
          );

endmodule
