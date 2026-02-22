module ex3(
    input [10:0] sw,
    output [3:0] led,
    output [7:0] seg0,
    output [2:0] flag_led
  );

  wire [3:0] alu_out;
  wire c, v, z;

  alu #(4) my_alu(
        .a(sw[3:0]), .b(sw[7:4]), .opt(sw[10:8]),
        .out(alu_out), .carry(c), .overflow(v), .zero(z)
      );

  assign led = alu_out;
  assign flag_led = {v, c, z};

  bcd7seg display(
            .b(alu_out),
            .h(seg0)
          );

endmodule
