module cpu(
    input clk,
    input rst,
    output reg [7:0] out_val,
    output [3:0] debug_pc,
    output [7:0] debug_r2
  );

  reg [3:0] pc;
  reg [7:0] inst;
  reg [7:0] rf [3:0];

  assign debug_pc = pc;
  assign debug_r2 = rf[2];

  // 指令存储器 (IMEM)
  // 放入你提供的汇编机器码
  always @(*)
    begin
      case(pc)
        4'h0:
          inst = 8'h8a; // li r0, 10
        4'h1:
          inst = 8'h90; // li r1, 0
        4'h2:
          inst = 8'ha0; // li r2, 0
        4'h3:
          inst = 8'hb1; // li r3, 1
        4'h4:
          inst = 8'h17; // add r1, r1, r3
        4'h5:
          inst = 8'h29; // add r2, r2, r1
        4'h6:
          inst = 8'hd1; // bner0 r1, 4
        4'h7:
          inst = 8'h42; // out r2
        default:
          inst = 8'h00;
      endcase
    end

  // 译码
  wire [1:0] op   = inst[7:6];
  wire [1:0] rd   = inst[5:4];
  wire [1:0] rs1  = inst[3:2];
  wire [1:0] rs2  = inst[1:0];
  wire [3:0] imm  = inst[3:0];
  wire [3:0] addr = inst[5:2];

  always @(posedge clk or posedge rst)
    begin
      if (rst)
        begin
          pc <= 4'h0;
          rf[0] <= 8'h0;
          rf[1] <= 8'h0;
          rf[2] <= 8'h0;
          rf[3] <= 8'h0;
          out_val <= 8'h0;
        end
      else
        begin
          case(op)
            2'b00:
              begin // add rd, rs1, rs2
                rf[rd] <= rf[rs1] + rf[rs2];
                pc <= pc + 1;
              end
            2'b01:
              begin // out rs2
                out_val <= rf[rs2];
                pc <= pc + 1;
              end
            2'b10:
              begin // li rd, imm
                rf[rd] <= {4'b0, imm};
                pc <= pc + 1;
              end
            2'b11:
              begin // bner0 rs2, addr
                if (rf[0] != rf[rs2])
                  pc <= addr;
                else
                  pc <= pc + 1;
              end
          endcase
        end
    end

endmodule
