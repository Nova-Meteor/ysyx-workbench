module ex2(
    input [8:0] sw,      // SW[7:0] 为输入, SW[8] 为使能
    output [3:0] led,    // LED[2:0] 为编码结果, LED[3] 为指示位
    output [7:0] seg0,    // 数码管 HEX0
    output [7:0] seg1,    // 数码管 HEX1
    output [7:0] seg2,    // 数码管 HEX2
    output [7:0] seg3,    // 数码管 HEX3
    output [7:0] seg4,    // 数码管 HEX4
    output [7:0] seg5,    // 数码管 HEX5
    output [7:0] seg6,    // 数码管 HEX6
    output [7:0] seg7     // 数码管 HEX7
  );

  wire [2:0] encode_out;
  wire indicator;

  encode83 my_encoder(
             .x(sw[7:0]),
             .en(sw[8]),
             .y(encode_out),
             .count(indicator)
           );

  assign led[2:0] = encode_out;
  assign led[3] = indicator;

  bcd7seg my_seg0(
            .b({1'b0, encode_out}),
            .h(seg0)
          );

  bcd7seg my_seg1(
            .b(4'b0),
            .h(seg1)
          );

  bcd7seg my_seg2(
            .b(4'b0),
            .h(seg2)
          );

  bcd7seg my_seg3(
            .b(4'b0),
            .h(seg3)
          );

  bcd7seg my_seg4(
            .b(4'b0),
            .h(seg4)
          );

  bcd7seg my_seg5(
            .b(4'b0),
            .h(seg5)
          );

  bcd7seg my_seg6(
            .b(4'b0),
            .h(seg6)
          );

  bcd7seg my_seg7(
            .b(4'b0),
            .h(seg7)
          );

endmodule
