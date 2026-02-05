#include <am.h>
#include <klib-macros.h>

void draw(uint32_t color) {
  // 获取屏幕尺寸
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  int w = config.width;
  int h = config.height;

  // 准备一个像素缓冲区
  uint32_t pixels[w * h];
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }

  // 把缓冲区画到屏幕上，并设置 sync 为 true 强制刷新
  io_write(AM_GPU_FBDRAW, 0, 0, pixels, w, h, true);
}

int main() {
  ioe_init(); // initialization for GUI
  while (1) {
    draw(0x000000ff);
  }
  return 0;
}
