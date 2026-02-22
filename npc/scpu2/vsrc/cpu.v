module cpu(
    input clk,
    input rst,
    output reg [7:0] out_val
  );

  reg [3:0] pc;
  reg [7:0] inst;

  // 核心改动 1：手动拆解寄存器堆，避免综合器产生奇怪的索引逻辑
  reg [7:0] rf0, rf1, rf2, rf3;

  // 指令存储器 (IMEM) 部分保持不变
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

  // 译码逻辑保持不变
  wire [1:0] op   = inst[7:6];
  wire [1:0] rd   = inst[5:4];
  wire [1:0] rs1  = inst[3:2];
  wire [1:0] rs2  = inst[1:0];
  wire [3:0] imm  = inst[3:0];
  wire [3:0] addr = inst[5:2];

  // 辅助逻辑：读取寄存器堆 (组合逻辑 MUX)
  wire [7:0] rf_rs1_val = (rs1 == 2'd0) ? rf0 : (rs1 == 2'd1) ? rf1 : (rs1 == 2'd2) ? rf2 : rf3;
  wire [7:0] rf_rs2_val = (rs2 == 2'd0) ? rf0 : (rs2 == 2'd1) ? rf1 : (rs2 == 2'd2) ? rf2 : rf3;

  // 核心改动 2：将异步复位改为物理设计更友好的“同步复位”风格
  // 并且所有写操作都基于全局 clk
  always @(posedge clk)
    begin
      if (rst)
        begin
          pc  <= 4'h0;
          rf0 <= 8'h0;
          rf1 <= 8'h0;
          rf2 <= 8'h0;
          rf3 <= 8'h0;
          out_val <= 8'h0;
        end
      else
        begin
          // 默认 PC 加 1
          pc <= pc + 4'h1;

          case(op)
            2'b00:
              begin // add rd, rs1, rs2
                if (rd == 2'd0)
                  rf0 <= rf_rs1_val + rf_rs2_val;
                if (rd == 2'd1)
                  rf1 <= rf_rs1_val + rf_rs2_val;
                if (rd == 2'd2)
                  rf2 <= rf_rs1_val + rf_rs2_val;
                if (rd == 2'd3)
                  rf3 <= rf_rs1_val + rf_rs2_val;
              end
            2'b01:
              begin // out rs2
                out_val <= rf_rs2_val;
              end
            2'b10:
              begin // li rd, imm
                if (rd == 2'd0)
                  rf0 <= {4'b0, imm};
                if (rd == 2'd1)
                  rf1 <= {4'b0, imm};
                if (rd == 2'd2)
                  rf2 <= {4'b0, imm};
                if (rd == 2'd3)
                  rf3 <= {4'b0, imm};
              end
            2'b11:
              begin // bner0 rs2, addr
                if (rf0 != rf_rs2_val)
                  pc <= addr;
              end
          endcase
        end
    end

endmodule
