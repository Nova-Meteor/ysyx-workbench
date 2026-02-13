module lfsr(
    input clk,
    input reset_n,
    output reg [7:0] q
  );

  wire feedback;

  assign feedback = q[4] ^ q[3] ^ q[2] ^ q[0];

  always @(posedge clk)
    begin
      if (!reset_n)
        begin
          q <= 8'h01;
        end
      else
        begin
          q <= {feedback, q[7:1]};
        end
    end

endmodule
