module ex7(
    input clk,
    input resetn,
    input ps2_clk,
    input ps2_data,
    output VGA_CLK,
    output [7:0] seg0, seg1, seg2, seg3, seg4, seg5
  );

  assign VGA_CLK = clk;

  wire [7:0] kbd_data;
  wire kbd_ready;

  ps2_keyboard my_kbd(
                 .clk(clk),
                 .resetn(resetn),
                 .ps2_clk(ps2_clk),
                 .ps2_data(ps2_data),
                 .out_code(kbd_data),
                 .ready(kbd_ready)
               );

  // 状态定义
  localparam STATE_IDLE    = 2'b00; // 等待按下
  localparam STATE_PRESSED = 2'b01; // 已按下，忽略重复码
  localparam STATE_WAIT_F0 = 2'b10; // 收到 F0，等待紧跟的释放码

  reg [1:0] state;
  reg [7:0] cur_scancode;
  reg [7:0] count;
  reg is_pressed;

  always @(posedge clk)
    begin
      if (!resetn)
        begin
          state <= STATE_IDLE;
          cur_scancode <= 8'h00;
          count <= 8'h00;
          is_pressed <= 1'b0;
        end
      else if (kbd_ready)
        begin
          case (state)
            STATE_IDLE:
              begin
                if (kbd_data != 8'hF0)
                  begin
                    state <= STATE_PRESSED;
                    cur_scancode <= kbd_data;
                    count <= count + 1'b1; // 只有从 IDLE 进入 PRESSED 才计数
                    is_pressed <= 1'b1;
                  end
                else
                  begin
                    state <= STATE_IDLE;
                  end
              end
            STATE_PRESSED:
              begin
                if (kbd_data == 8'hF0)
                  begin
                    state <= STATE_WAIT_F0;
                  end
                else
                  begin
                    state <= STATE_PRESSED;
                  end
                // 如果收到的是重复的通码，保持在 PRESSED 状态，不计数
              end
            STATE_WAIT_F0:
              begin
                // 收到 F0 后的下一个码，代表彻底松开
                state <= STATE_IDLE;
                is_pressed <= 1'b0;
                cur_scancode <= 8'h00;
              end
            default:
              state <= STATE_IDLE;
          endcase
        end
    end

  // ASCII 转换
  wire [7:0] cur_ascii;
  scancode_to_ascii my_map(cur_scancode, cur_ascii);

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
  bcd7seg seg_3(cur_ascii[7:4], h3_raw);
  bcd7seg seg_2(cur_ascii[3:0], h2_raw);
  bcd7seg seg_1(cur_scancode[7:4], h1_raw);
  bcd7seg seg_0(cur_scancode[3:0], h0_raw);

endmodule
