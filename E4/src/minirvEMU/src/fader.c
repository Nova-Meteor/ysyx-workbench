#include <am.h>
#include <klib.h>
#include <klib-macros.h>

// 目标颜色库
uint32_t colors[] = {
  0x000000, 0xff0000, 0x00ff00, 0x0000ff, 
  0xffff00, 0xff00ff, 0x00ffff, 0xffffff
};

// 速度控制变量：正常延迟为 20ms，加速延迟为 2ms
int step_delay = 20;

// 按键处理函数
void check_keys() {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode != AM_KEY_NONE) {
    if (ev.keydown) {
      // 如果按下的是 ESC 键，调用 halt 退出程序
      if (ev.keycode == AM_KEY_ESCAPE) {
        halt(0); 
      }
      // 按下其他任意键，将延迟缩短（加快速度）
      step_delay = 2; 
    } else {
      // 释放按键，恢复正常延迟
      step_delay = 20;
    }
  }
}

void draw_full_screen(uint32_t color) {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  int w = config.width, h = config.height;
  
  uint32_t buf[w]; 
  for (int i = 0; i < w; i++) buf[i] = color;
  for (int y = 0; y < h; y++) {
    io_write(AM_GPU_FBDRAW, 0, y, buf, w, 1, true);
  }
}

int main() {
  ioe_init();

  int k = 100;           // 一轮渐变的步数
  int color_idx = 0;
  uint32_t curr_c = colors[0];
  
  while (1) {
    color_idx = (color_idx + 1) % (sizeof(colors) / sizeof(uint32_t));
    uint32_t target_c = colors[color_idx];

    int r0 = (curr_c >> 16) & 0xff, g0 = (curr_c >> 8) & 0xff, b0 = curr_c & 0xff;
    int rk = (target_c >> 16) & 0xff, gk = (target_c >> 8) & 0xff, bk = target_c & 0xff;

    // 开始一轮渐变
    for (int i = 1; i <= k; i++) {
      // 1. 每一步都检测按键状态
      check_keys();

      int ri = r0 + (rk - r0) * i / k;
      int gi = g0 + (gk - g0) * i / k;
      int bi = b0 + (bk - b0) * i / k;

      uint32_t color = (ri << 16) | (gi << 8) | bi;
      draw_full_screen(color);

      // 2. 使用受按键影响的 step_delay 控制等待时间
      uint64_t start = io_read(AM_TIMER_UPTIME).us;
      while (io_read(AM_TIMER_UPTIME).us - start < step_delay * 1000) {
        // 在等待期间持续轮询按键，以提高响应灵敏度
        check_keys();
      }
    }

    curr_c = target_c;
  }
  return 0;
}
