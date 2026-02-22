# 定义时钟：周期为 20ns (对应 50MHz)，时钟端口名为 clk
create_clock -name core_clock -period 20 [get_ports clk]
