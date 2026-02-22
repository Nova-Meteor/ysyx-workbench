module adder #(parameter WIDTH = 4) (
    input [WIDTH-1:0] a,
    input [WIDTH-1:0] b,
    input sub,              // 0为加法，1为减法
    output [WIDTH-1:0] s,
    output carry,           // 进位标识 C
    output overflow,        // 溢出标识 V
    output zero             // 零标识 Z
  );

  wire [WIDTH-1:0] b_mux = sub ? ~b : b;
  // 使用 Verilog 的拼接符实现带进位的加法
  assign {carry, s} = a + b_mux + { {(WIDTH-1){1'b0}}, sub };

  // 溢出判断：两个正数相加得负数，或两个负数相加得正数
  assign overflow = (a[WIDTH-1] == b_mux[WIDTH-1]) && (s[WIDTH-1] != a[WIDTH-1]);
  //   assign zero = (s == 0);
  assign zero = ~|s;

endmodule
