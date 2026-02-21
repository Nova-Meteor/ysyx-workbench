/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int buf_pos = 0;

static uint32_t choose(uint32_t n) {
  return rand() % n;
}

static void gen(char c) {
  if (buf_pos < sizeof(buf) - 1) {
    buf[buf_pos++] = c;
    buf[buf_pos] = '\0';
  }
}

static void gen_num() {
  uint32_t num = choose(100);
  char num_str[16];
  sprintf(num_str, "%u", num);
  
  int len = strlen(num_str);
  if (buf_pos + len < sizeof(buf) - 1) {
    strcpy(buf + buf_pos, num_str);
    buf_pos += len;
  }
}

static void gen_space() {
  if (choose(2) == 0) return;  // 50%概率不加空格
  int spaces = choose(3) + 1;   // 1~3个空格
  for (int i = 0; i < spaces && buf_pos < sizeof(buf) - 1; i++) {
    buf[buf_pos++] = ' ';
  }
  buf[buf_pos] = '\0';
}

// 递归生成表达式
// 添加参数限制递归深度，防止栈溢出和buf溢出
static void gen_expr(int depth) {
  // 限制递归深度，强制终止
  if (depth > 10) {
    gen_num();
    return;
  }
  
  // 限制buf剩余空间，避免溢出
  if (buf_pos > sizeof(buf) - 32) {
    gen_num();
    return;
  }

  switch (choose(3)) {
    case 0:  // 生成数字
      gen_space();
      gen_num();
      gen_space();
      break;
      
    case 1:  // 生成括号表达式
      gen_space();
      gen('(');
      gen_expr(depth + 1);
      gen(')');
      gen_space();
      break;
      
    default: {  // 生成二元运算表达式
      gen_space();

      // 先生成左子表达式
      gen_expr(depth + 1);

      gen_space();

      // 生成运算符（避免除法除零）
      char op;
      if (choose(4) == 0) {
        // 25%概率生成除法，右操作数需要特殊处理
        op = '/';
        gen(op);
        gen_space();

        // 右操作数必须是非零数字（简化：直接生成1-99的非零数）
        uint32_t num = choose(99) + 1;  // 1~99
        char num_str[16];
        sprintf(num_str, "%u", num);
        int len = strlen(num_str);
        if (buf_pos + len < sizeof(buf) - 1) {
          strcpy(buf + buf_pos, num_str);
          buf_pos += len;
        }
      } else {
        // 75%概率生成非除法运算符
        op = "+-*"[choose(3)];
        gen(op);
        gen_space();

        // 生成右子表达式
        gen_expr(depth + 1);
      }

      gen_space();
      break;
    }
  }
}

static void gen_rand_expr() {
  buf_pos = 0;
  buf[0] = '\0';
  gen_expr(0);
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
