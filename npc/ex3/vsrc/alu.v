module alu #(parameter WIDTH = 4) (
    input [WIDTH-1:0] a,
    input [WIDTH-1:0] b,
    input [2:0] opt,        // 操作码：000加, 001减, 010非, 011与, 100或, 101异或, 110小于比较, 111等于比较
    output reg [WIDTH-1:0] out,
    output carry,
    output overflow,
    output zero
  );

  wire [WIDTH-1:0] add_sub_res;

  adder #(WIDTH) add_unit (
          .a(a), .b(b), .sub(opt[0] | opt[2]),
          .s(add_sub_res), .carry(carry), .overflow(overflow), .zero(zero)
        );

  always @(*)
    begin
      case(opt)
        3'b000:
          out = add_sub_res;          // Add
        3'b001:
          out = add_sub_res;          // Sub
        3'b010:
          out = ~a;                   // Not
        3'b011:
          out = a & b;                // And
        3'b100:
          out = a | b;                // Or
        3'b101:
          out = a ^ b;                // Xor
        3'b110:
          out = { {(WIDTH-1){1'b0}}, (a[WIDTH-1] != b[WIDTH-1]) ? a[WIDTH-1] : add_sub_res[WIDTH-1] }; // SLT (带符号)
        3'b111:
          out = { {(WIDTH-1){1'b0}}, zero }; // Equ
        default:
          out = 0;
      endcase
    end
endmodule
