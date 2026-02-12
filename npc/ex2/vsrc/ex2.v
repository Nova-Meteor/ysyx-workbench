module ex2(
    input [8:0] sw,      // SW[7:0] 为输入, SW[8] 为使能
    output [3:0] led,    // LED[2:0] 为编码结果, LED[4] 为指示位
    output [7:0] seg0    // 数码管 HEX0
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

  bcd7seg my_seg(
            .b({1'b0, encode_out}),
            .h(seg0)
          );

endmodule
