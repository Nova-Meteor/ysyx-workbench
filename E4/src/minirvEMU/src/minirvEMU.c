#include <stdio.h> 
#include <stdint.h>
#include <am.h>
#include <klib.h>
#include <klib-macros.h>

// --- 1. 架构定义 ---
#define MEM_SIZE (4 * 1024 * 1024)
uint32_t PC = 0;
uint32_t R[32] = {0};
uint8_t M[MEM_SIZE];

// --- VGA 帧缓冲区定义 ---
// 根据提示，地址空间为 [0x20000000, 0x20040000)
#define VGA_BASE 0x20000000
#define VGA_END  0x20040000
#define VGA_SIZE (VGA_END - VGA_BASE)
uint8_t VGA_MEM[VGA_SIZE]; // 存放像素信息

// --- 2. 辅助宏：字段提取 ---
#define OPCODE(inst)  ((inst) & 0x7F)
#define RD(inst)      (((inst) >> 7) & 0x1F)
#define RS1(inst)     (((inst) >> 15) & 0x1F)
#define RS2(inst)     (((inst) >> 20) & 0x1F)
#define FUNC3(inst)   (((inst) >> 12) & 0x7)
#define FUNC7(inst)   (((inst) >> 25) & 0x7F)

#define IMM_I(inst)   ((int32_t)(inst) >> 20)
#define IMM_S(inst)   (((int32_t)(inst & 0xFE000000) >> 20) | (((inst) >> 7) & 0x1F))
#define IMM_U(inst)   ((inst) & 0xFFFFF000)

// --- 3. 指令周期 ---
void inst_cycle() {
    if (PC >= MEM_SIZE) return;
    
    uint32_t inst = *(uint32_t*)&M[PC];
    
    uint8_t opcode = OPCODE(inst);
    uint8_t rd = RD(inst);
    uint8_t rs1 = RS1(inst);
    uint8_t rs2 = RS2(inst);
    uint8_t func3 = FUNC3(inst);
    uint8_t func7 = FUNC7(inst);

    R[0] = 0;
    uint32_t next_PC = PC + 4;
    
    switch (opcode) {
        case 0x33: // R-Type
            if (func3 == 0x0 && func7 == 0x00) R[rd] = R[rs1] + R[rs2];
            break;

        case 0x13: // I-Type (addi)
            if (func3 == 0x0) R[rd] = R[rs1] + IMM_I(inst);
            break;

        case 0x03: // Loads
            {
                uint32_t addr = R[rs1] + IMM_I(inst);
                if (addr < MEM_SIZE) {
                    if (func3 == 0x2) R[rd] = *(int32_t*)&M[addr];
                    else if (func3 == 0x4) R[rd] = M[addr];
                }
            }
            break;

        case 0x23: // Stores (重点：判断 VGA 地址)
            {
                uint32_t addr = R[rs1] + IMM_S(inst);
                // 检查是否落在像素缓冲区地址段
                if (addr >= VGA_BASE && addr < VGA_END) {
                    uint32_t vga_offset = addr - VGA_BASE;
                    if (func3 == 0x2) { // sw
                        *(uint32_t*)&VGA_MEM[vga_offset] = R[rs2];
                    } else if (func3 == 0x0) { // sb
                        VGA_MEM[vga_offset] = R[rs2] & 0xFF;
                    }
                } else if (addr < MEM_SIZE) {
                    if (func3 == 0x2) *(uint32_t*)&M[addr] = R[rs2];
                    else if (func3 == 0x0) M[addr] = R[rs2] & 0xFF;
                }
            }
            break;

        case 0x37: // lui
            R[rd] = IMM_U(inst);
            break;

        case 0x67: // jalr
            if (func3 == 0x0) {
                uint32_t target = (R[rs1] + IMM_I(inst)) & ~1;
                if (rd != 0) R[rd] = PC + 4;
                next_PC = target;
            }
            break;
        
        case 0x6F: // jal
            if (rd != 0) R[rd] = PC + 4;
            next_PC = PC + (((int32_t)((inst & 0x80000000)) >> 11) | (inst & 0xFF000) | ((inst >> 9) & 0x800) | ((inst >> 20) & 0x7FE));
            break;
    }

    R[0] = 0;
    PC = next_PC;
}

int main(const char *args) {
    // 1. 初始化 IOE
    ioe_init();

    // 2. 处理参数
    if (args == NULL || strlen(args) == 0) {
        printf("Usage: make ARCH=native run mainargs=xxx.bin\n");
        return -1;
    }

    const char *filename = args;
    printf("Attempting to open file: %s\n", filename);

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: Cannot open file '%s'. Please check the path.\n", filename);
        return -1;
    }

    // 3. 读取文件
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size > MEM_SIZE) size = MEM_SIZE;
    size_t read_size = fread(M, 1, size, fp);
    fclose(fp);
    
    printf("Successfully loaded %zu bytes.\n", read_size);

    // --- 运行模拟器 ---
    uint32_t last_PC = 0;
    int limit = 20000000;
    while (limit-- > 0) {
        last_PC = PC;
        inst_cycle();
        if (PC == last_PC) break;
    }

    // --- 4. 显示 256*256 图像 ---
    int img_w = 256;
    int img_h = 256;
    io_write(AM_GPU_FBDRAW, 0, 0, VGA_MEM, img_w, img_h, true);

    // 保持显示，防止程序退出
    while (1);

    return 0;
}
