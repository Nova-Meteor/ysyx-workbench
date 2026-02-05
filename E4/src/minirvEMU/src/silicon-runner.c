#include <am.h>
#include <klib.h>
#include <klib-macros.h>

// --- 配置与常量 ---
#define FPS            30
#define SCALE          1024
#define GRAVITY        500
#define JUMP_FORCE     -8000
#define CHAR_W         8
#define CHAR_H         16
#define COL_PURPLE     0x2a0a29
#define COL_WHITE      0xeeeeee
#define COL_RED        0xff0033

extern char font[];

// --- 游戏状态结构体 ---
struct {
  int x, y, v;       // 玩家位置(x, y)和速度(v)，y和v使用定点数
  int obs_x, obs_y;  // 障碍物位置
  int screen_w, screen_h;
  uint64_t last_act; // 上次按键时间
  bool dead;
  bool on_ground;
} game;

// 图像缓冲区
uint32_t char_texture[26][CHAR_W * CHAR_H];
uint32_t blank[CHAR_W * CHAR_H];

// --- 初始化纹理 ---
void init_textures() {
  for (int i = 0; i < CHAR_W * CHAR_H; i++) blank[i] = COL_PURPLE;
  for (int ch = 0; ch < 26; ch++) {
    char *p = &font[CHAR_H * ch];
    for (int y = 0; y < CHAR_H; y++) {
      for (int x = 0; x < CHAR_W; x++) {
        int bit = (p[y] >> (CHAR_W - x - 1)) & 1;
        char_texture[ch][y * CHAR_W + x] = bit ? COL_WHITE : COL_PURPLE;
      }
    }
  }
}

// --- 初始化游戏状态 ---
void init_game() {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  game.screen_w = config.width;
  game.screen_h = config.height;
  game.x = 20;
  game.y = (game.screen_h - CHAR_H - 10) * SCALE;
  game.v = 0;
  game.obs_x = game.screen_w;
  game.obs_y = game.screen_h - CHAR_H - 10;
  game.last_act = io_read(AM_TIMER_UPTIME).us;
  game.dead = false;
  game.on_ground = true;

  // 初始全屏紫色
  uint32_t line[game.screen_w];
  for (int i = 0; i < game.screen_w; i++) line[i] = COL_PURPLE;
  for (int y = 0; y < game.screen_h; y++)
    io_write(AM_GPU_FBDRAW, 0, y, line, game.screen_w, 1, false);
}

// --- 逻辑更新 ---
void update_logic() {
  if (game.dead) return;

  // 1. 物理计算 (定点数)
  game.v += GRAVITY;
  game.y += game.v;

  // 落地判定
  int ground_line = (game.screen_h - CHAR_H - 10) * SCALE;
  if (game.y >= ground_line) {
    game.y = ground_line;
    game.v = 0;
    game.on_ground = true;
  } else {
    game.on_ground = false;
  }

  // 2. 障碍物移动
  game.obs_x -= 6;
  if (game.obs_x < 0) game.obs_x = game.screen_w;

  // 3. 碰撞检测
  int py = game.y / SCALE;
  if (game.obs_x >= game.x && game.obs_x <= game.x + CHAR_W) {
    if (abs(py - game.obs_y) < CHAR_H - 2) game.dead = true;
  }

  // 4. 消极失败检测 (5秒不操作)
  if (io_read(AM_TIMER_UPTIME).us - game.last_act > 5000000) game.dead = true;
}

// --- 局部刷新渲染 ---
void render() {
  static int last_px = 0, last_py = 0, last_ox = 0, last_oy = 0;

  // 擦除上一帧物体位置
  io_write(AM_GPU_FBDRAW, last_px, last_py, blank, CHAR_W, CHAR_H, false);
  io_write(AM_GPU_FBDRAW, last_ox, last_oy, blank, CHAR_W, CHAR_H, false);

  // 更新当前位置记录
  last_px = game.x;
  last_py = game.y / SCALE;
  last_ox = game.obs_x;
  last_oy = game.obs_y;

  // 绘制玩家 'C' 和 障碍物 'X' (红色)
  io_write(AM_GPU_FBDRAW, last_px, last_py, char_texture['C' - 'A'], CHAR_W, CHAR_H, false);
  
  // 临时修改障碍物纹理为红色以便区分
  for(int i=0; i<CHAR_W*CHAR_H; i++) {
    if(char_texture['X'-'A'][i] == COL_WHITE) char_texture['X'-'A'][i] = COL_RED;
  }
  io_write(AM_GPU_FBDRAW, last_ox, last_oy, char_texture['X' - 'A'], CHAR_W, CHAR_H, false);

  // 统一刷新显示
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
}

// --- 主循环 ---
int main() {
  ioe_init();
  init_textures();
  init_game();

  uint64_t t0 = io_read(AM_TIMER_UPTIME).us;
  int current_frame = 0, rendered_frame = 0;

  while (1) {
    int frames = (io_read(AM_TIMER_UPTIME).us - t0) / (1000000 / FPS);

    // 逻辑步进
    for (; current_frame < frames; current_frame++) {
      update_logic();
    }

    // 事件处理
    while (1) {
      AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
      if (ev.keycode == AM_KEY_NONE) break;
      if (ev.keydown) {
        game.last_act = io_read(AM_TIMER_UPTIME).us; // 刷新活动时间
        
        if (ev.keycode == AM_KEY_ESCAPE) halt(0);
        
        if (ev.keycode == AM_KEY_SPACE) {
          if (game.on_ground && !game.dead) {
            game.v = JUMP_FORCE;
          }
        }
        
        if (game.dead) init_game(); // 死亡后按任意键重启
      }
    }

    // 渲染
    if (current_frame > rendered_frame) {
      render();
      rendered_frame = current_frame;
    }
  }
  return 0;
}