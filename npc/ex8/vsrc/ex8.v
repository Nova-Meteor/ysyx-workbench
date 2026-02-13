module ex8(
    input clk,
    input rst,
    output VGA_CLK,
    output VGA_HSYNC,
    output VGA_VSYNC,
    output VGA_BLANK_N,
    output [7:0] VGA_R,
    output [7:0] VGA_G,
    output [7:0] VGA_B
  );

  assign VGA_CLK = clk;

  wire [9:0] h_addr;
  wire [9:0] v_addr;
  wire [23:0] vga_data;

  vga_ctrl my_vga_ctrl(
             .pclk(clk),
             .reset(rst),
             .vga_data(vga_data),
             .h_addr(h_addr),
             .v_addr(v_addr),
             .hsync(VGA_HSYNC),
             .vsync(VGA_VSYNC),
             .valid(VGA_BLANK_N),
             .vga_r(VGA_R),
             .vga_g(VGA_G),
             .vga_b(VGA_B)
           );

  vmem my_vmem(
         .h_addr(h_addr),
         .v_addr(v_addr),
         .vga_data(vga_data)
       );

endmodule

module vmem(
    input [9:0] h_addr,
    input [9:0] v_addr,
    output [23:0] vga_data
  );

  // 优化：内存深度减小为实际像素数 (582 * 535 = 311,370)
  // reg [23:0] vga_mem [1048575:0];
  reg [23:0] vga_mem [311369:0];

  initial
    begin
      $readmemh("/home/wiki/ysyx/npc/ex8/resource/ysyx.hex", vga_mem);
    end

  // 优化：使用线性索引。FPGA 综合器通常会自动将乘法优化为加法和移位的组合。
  // assign vga_data = vga_mem[{h_addr, v_addr}];
  // 关键：将索引计算扩展到 19 位，防止地址溢出截断
  assign vga_data = vga_mem[{{22{1'b0}}, v_addr} * 32'd582 + {{22{1'b0}}, h_addr}];

endmodule
