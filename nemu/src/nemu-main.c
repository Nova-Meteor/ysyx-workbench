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

#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
word_t expr(char *e, bool *success);

// 表达式测试模式
static int test_expr(const char *input_file) {
  FILE *fp = fopen(input_file, "r");
  if (fp == NULL) {
    printf("Cannot open input file: %s\n", input_file);
    return 1;
  }

  char line[65536];
  int passed = 0, failed = 0;

  while (fgets(line, sizeof(line), fp) != NULL) {
    // 去除换行符
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }
    if (len == 0) continue;  // 跳过空行

    // 解析格式: "结果 表达式"
    // 找到第一个空格
    char *space = strchr(line, ' ');
    if (space == NULL) continue;

    unsigned int expected;
    sscanf(line, "%u", &expected);

    char *expr_str = space + 1;

    // 求值
    bool success;
    word_t result = expr(expr_str, &success);

    if (!success) {
      printf("FAILED (parse error): %s\n", expr_str);
      failed++;
    } else if (result != expected) {
      printf("FAILED: %s\n", expr_str);
      printf("  Expected: %u\n", expected);
      printf("  Got:      %u\n", result);
      failed++;
    } else {
      passed++;
    }
  }

  fclose(fp);
  printf("\nTest Summary: %d passed, %d failed\n", passed, failed);
  return failed > 0 ? 1 : 0;
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  // 检查是否是表达式测试模式
  if (argc >= 3 && strcmp(argv[1], "-e") == 0) {
    return test_expr(argv[2]);
  }

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
