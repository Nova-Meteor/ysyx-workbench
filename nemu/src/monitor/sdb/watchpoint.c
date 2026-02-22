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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[256];      // 监视的表达式
  word_t old_val;      // 表达式上一次的值

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (free_ == NULL) {
    assert(0);  // 没有空闲监视点，终止程序
  }

  // 从 free_ 链表头部取出一个节点
  WP *wp = free_;
  free_ = free_->next;

  // 将节点插入到 head 链表头部
  wp->next = head;
  head = wp;

  return wp;
}

void free_wp(WP *wp) {
  // 从 head 链表中移除 wp
  if (head == wp) {
    // wp 是头节点
    head = wp->next;
  } else {
    // 查找 wp 的前驱节点
    WP *prev = head;
    while (prev != NULL && prev->next != wp) {
      prev = prev->next;
    }
    if (prev == NULL) {
      return;  // wp 不在 head 链表中
    }
    prev->next = wp->next;
  }

  // 将 wp 归还到 free_ 链表头部
  wp->next = free_;
  free_ = wp;
}

// 添加监视点：根据表达式创建监视点
int wp_add(char *expr_str) {
  WP *wp = new_wp();
  strncpy(wp->expr, expr_str, sizeof(wp->expr) - 1);
  wp->expr[sizeof(wp->expr) - 1] = '\0';

  // 计算表达式的初始值
  bool success;
  wp->old_val = expr(wp->expr, &success);
  if (!success) {
    free_wp(wp);
    return -1;  // 表达式求值失败
  }

  return wp->NO;
}

// 删除监视点：根据编号删除
bool wp_remove(int no) {
  WP *wp = head;
  while (wp != NULL) {
    if (wp->NO == no) {
      free_wp(wp);
      return true;
    }
    wp = wp->next;
  }
  return false;  // 未找到该编号的监视点
}

// 检查节点是否在使用中
static bool is_in_use(WP *wp) {
  WP *p = head;
  while (p != NULL) {
    if (p == wp) return true;
    p = p->next;
  }
  return false;
}

// 列出所有监视点
void wp_list() {
  if (head == NULL) {
    printf("No watchpoints.\n");
    return;
  }

  printf("Num       Value           What\n");
  // 按 NO 顺序遍历 wp_pool
  for (int i = 0; i < NR_WP; i++) {
    if (is_in_use(&wp_pool[i])) {
      printf("%-8d  0x%08x      %s\n", wp_pool[i].NO, wp_pool[i].old_val, wp_pool[i].expr);
    }
  }
}

// 检查所有监视点，返回是否触发了监视点
bool wp_check() {
  bool triggered = false;
  WP *wp = head;

  while (wp != NULL) {
    bool success;
    word_t new_val = expr(wp->expr, &success);

    if (success && new_val != wp->old_val) {
      printf("\nWatchpoint %d: %s\n", wp->NO, wp->expr);
      printf("Old value = 0x%08x\n", wp->old_val);
      printf("New value = 0x%08x\n", new_val);
      wp->old_val = new_val;
      triggered = true;
    }

    wp = wp->next;
  }

  return triggered;
}
