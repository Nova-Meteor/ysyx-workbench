from PIL import Image

def convert_to_hex(image_path, output_path):
    # 1. 打开并转换图片为 RGB 格式
    img = Image.open(image_path).convert('RGB')
    width, height = img.size
    print(f"处理图片: {width}x{height}")

    # 2. 根据寻址逻辑确定总空间
    # h_addr 是 10 位，v_addr 是 10 位，总共 20 位寻址空间 (2^20)
    total_size = 1 << 20 
    mem = ["000000"] * total_size # 初始化背景为黑色

    # 3. 填充像素数据
    pixels = img.load()
    for x in range(width):
        for y in range(height):
            r, g, b = pixels[x, y]
            # 计算 Verilog 中的拼接地址: {h_addr, v_addr}
            # 等同于 (x << 10) | y
            addr = (x << 10) | y
            
            # 将 RGB 转换为 6 位十六进制字符串
            hex_val = f"{r:02X}{g:02X}{b:02X}"
            mem[addr] = hex_val

    # 4. 写入文件
    with open(output_path, 'w') as f:
        f.write("@000000\n") # 起始地址
        for val in mem:
            f.write(val + "\n")

    print(f"转换完成！文件已保存至: {output_path}")

if __name__ == "__main__":
    # 在这里修改你的文件名
    convert_to_hex("ysyx.png", "ysyx.hex")