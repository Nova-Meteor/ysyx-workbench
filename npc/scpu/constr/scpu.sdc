# 定义时钟：周期为 2ns (对应 500MHz)，时钟端口名为 clk
create_clock -name core_clock -period 2 [get_ports clk]
