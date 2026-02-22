module ex7(
    input clk,
    input rst,
    input ps2_clk,
    input ps2_data,
    output VGA_CLK,
    output [15:0] led,
    output [7:0] seg0, seg1, seg2, seg3, seg4, seg5
  );

  assign VGA_CLK = clk;

  wire [7:0] kbd_data;
  wire kbd_ready;

  ps2_keyboard my_kbd(
                 .clk(clk),
                 .resetn(~rst),
                 .ps2_clk(ps2_clk),
                 .ps2_data(ps2_data),
                 .out_code(kbd_data),
                 .ready(kbd_ready)
               );

  // 状态定义
  localparam STATE_IDLE    = 2'b00;
  localparam STATE_WAIT_F0 = 2'b10;

  reg [1:0] state;
  reg [7:0] cur_scancode;
  reg [7:0] count;
  reg is_pressed;

  // --- 组合键标志位 ---
  reg shift_on;
  reg ctrl_on;

  always @(posedge clk)
    begin
      if (rst)
        begin
          state <= STATE_IDLE;
          cur_scancode <= 8'h00;
          count <= 8'h00;
          is_pressed <= 1'b0;
          shift_on <= 1'b0;
          ctrl_on <= 1'b0;
        end
      else if (kbd_ready)
        begin
          case (state)
            STATE_IDLE:
              begin
                if (kbd_data == 8'hF0)
                  begin
                    state <= STATE_WAIT_F0;
                  end
                else if (kbd_data == 8'h12 || kbd_data == 8'h59)
                  begin
                    shift_on <= 1'b1;
                  end
                else if (kbd_data == 8'h14)
                  begin
                    ctrl_on <= 1'b1;
                  end
                else
                  begin
                    if (kbd_data != cur_scancode || !is_pressed)
                      begin
                        count <= count + 1'b1;
                      end
                    state <= STATE_IDLE;
                    cur_scancode <= kbd_data;
                    is_pressed <= 1'b1;
                  end
              end
            STATE_WAIT_F0:
              begin
                if (kbd_data == 8'h12 || kbd_data == 8'h59)
                  begin
                    shift_on <= 1'b0; // Shift 松开
                  end
                else if (kbd_data == 8'h14)
                  begin
                    ctrl_on <= 1'b0;  // CTRL 松开
                  end
                else if (kbd_data == cur_scancode)
                  begin
                    is_pressed <= 1'b0; // 当前显示的功能键松开
                  end
                state <= STATE_IDLE;
              end
            default:
              state <= STATE_IDLE;
          endcase
        end
    end

  assign led[0] = shift_on;
  assign led[1] = ctrl_on;
  assign led[15:2] = 14'b0;

  // ASCII 转换
  wire [7:0] raw_ascii;
  scancode_to_ascii my_map(cur_scancode, raw_ascii);

  wire [7:0] final_ascii = (raw_ascii >= 8'h41 && raw_ascii <= 8'h5A)
       ? (shift_on ? raw_ascii : raw_ascii + 8'h20)
       : raw_ascii;

  // --- 显示控制逻辑 ---
  // 高两位 (seg5, 4): 显示 count
  // 中两位 (seg3, 2): 显示 ASCII (仅按下时)
  // 低两位 (seg1, 0): 显示 ScanCode (仅按下时)

  wire [7:0] h3_raw, h2_raw, h1_raw, h0_raw;

  assign seg3 = is_pressed ? h3_raw : 8'hFF;
  assign seg2 = is_pressed ? h2_raw : 8'hFF;
  assign seg1 = is_pressed ? h1_raw : 8'hFF;
  assign seg0 = is_pressed ? h0_raw : 8'hFF;

  bcd7seg seg_5(count[7:4], seg5);
  bcd7seg seg_4(count[3:0], seg4);
  bcd7seg seg_3(final_ascii[7:4], h3_raw);
  bcd7seg seg_2(final_ascii[3:0], h2_raw);
  bcd7seg seg_1(cur_scancode[7:4], h1_raw);
  bcd7seg seg_0(cur_scancode[3:0], h0_raw);

endmodule
