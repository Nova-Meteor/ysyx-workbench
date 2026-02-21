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
#include <stdbool.h>
#include <stdlib.h>
#include <isa.h>
#include <memory/vaddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUM,
  TK_HEX,
  TK_REG,
  TK_NEG,
  TK_DEREF,
  TK_NE,
  TK_AND,

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},      // spaces
  {"\\+", '+'},           // plus
  {"-", '-'},             // minus
  {"\\*", '*'},           // multiply / dereference
  {"/", '/'},             // divide

  {"\\(", '('},           // left parenthesis
  {"\\)", ')'},           // right parenthesis

  {"0x[0-9a-fA-F]+", TK_HEX},  // hexadecimal numbers
  {"[0-9]+", TK_NUM},         // decimal numbers
  {"\\$[a-zA-Z0-9]+", TK_REG}, // registers

  {"==", TK_EQ},          // equal
  {"!=", TK_NE},          // not equal
  {"&&", TK_AND},         // logical and
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (rules[i].token_type == TK_NOTYPE) {
          break;
        }

        if (nr_token >= ARRLEN(tokens)) {
          printf("Error: too many tokens (max %d)\n", ARRLEN(tokens));
          return false;
        }

        if (substr_len >= sizeof(tokens[nr_token].str)) {
          printf("Error: token too long (max %ld)\n", sizeof(tokens[nr_token].str) - 1);
          return false;
        }

        tokens[nr_token].type = rules[i].token_type;

        strncpy(tokens[nr_token].str, substr_start, substr_len);
        tokens[nr_token].str[substr_len] = '\0';

        nr_token ++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // 处理一元运算符：解引用（*）和负号（-）
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '*') {
      // 解引用：在开头或前一个token是运算符/括号
      if (i == 0 ||
          tokens[i - 1].type == '+' ||
          tokens[i - 1].type == '-' ||
          tokens[i - 1].type == '*' ||
          tokens[i - 1].type == '/' ||
          tokens[i - 1].type == '(' ||
          tokens[i - 1].type == TK_EQ ||
          tokens[i - 1].type == TK_NE ||
          tokens[i - 1].type == TK_AND ||
          tokens[i - 1].type == TK_NEG ||
          tokens[i - 1].type == TK_DEREF) {
        tokens[i].type = TK_DEREF;
      }
    }
    if (tokens[i].type == '-') {
      // 负号：在开头或前一个token是运算符/括号
      if (i == 0 ||
          tokens[i - 1].type == '+' ||
          tokens[i - 1].type == '-' ||
          tokens[i - 1].type == '*' ||
          tokens[i - 1].type == '/' ||
          tokens[i - 1].type == '(' ||
          tokens[i - 1].type == TK_EQ ||
          tokens[i - 1].type == TK_NE ||
          tokens[i - 1].type == TK_AND ||
          tokens[i - 1].type == TK_NEG ||
          tokens[i - 1].type == TK_DEREF) {
        tokens[i].type = TK_NEG;
      }
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
  if (tokens[p].type != '(' || tokens[q].type != ')') {
    return false;
  }
  
  // 检查括号是否匹配
  int cnt = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') cnt++;
    else if (tokens[i].type == ')') cnt--;
    
    // 如果在到达q之前括号就匹配完了，说明最外层括号不匹配
    if (cnt == 0 && i < q) return false;
  }
  
  return cnt == 0;
}

static int get_priority(int op) {
  switch (op) {
    case TK_AND: return 1;      // && 最低
    case TK_EQ:
    case TK_NE: return 2;       // == !=
    case '+':
    case '-': return 3;         // + -
    case '*':
    case '/': return 4;         // * /
    case TK_NEG:
    case TK_DEREF: return 5;    // 一元运算符最高
    default: return 0;
  }
}

static uint32_t eval(int p, int q, bool *success) {
  *success = true;

  if (p > q) {
    // 空表达式，错误
    *success = false;
    return 0;
  }

  else if (p == q) {
    // 单个token
    if (tokens[p].type == TK_NUM) {
      return atoi(tokens[p].str);
    }
    if (tokens[p].type == TK_HEX) {
      return (uint32_t)strtol(tokens[p].str, NULL, 16);
    }
    if (tokens[p].type == TK_REG) {
      return isa_reg_str2val(tokens[p].str, success);
    }
    *success = false;
    return 0;
  }

  else if (check_parentheses(p, q)) {
    // 被括号包围，去掉括号递归
    return eval(p + 1, q - 1, success);
  }

  else {
    // 找主运算符（优先级最低，且在括号外的二元运算符）
    int op_pos = -1;
    int min_priority = 10;  // 比所有运算符优先级都高

    for (int i = p; i <= q; i++) {
      // 跳过括号内的内容
      if (tokens[i].type == '(') {
        int cnt = 1;
        i++;
        while (i <= q && cnt > 0) {
          if (tokens[i].type == '(') cnt++;
          else if (tokens[i].type == ')') cnt--;
          i++;
        }
        i--;  // 回退一位，因为for循环会i++
        continue;
      }

      // 一元运算符不作为主运算符
      if (tokens[i].type == TK_NEG || tokens[i].type == TK_DEREF) {
        continue;
      }

      int priority = get_priority(tokens[i].type);
      // 优先级更低，或同级但靠右
      if (priority > 0 && priority <= min_priority) {
        min_priority = priority;
        op_pos = i;
      }
    }

    // 如果没找到二元运算符，检查是否是一元运算符表达式
    if (op_pos == -1) {
      if (tokens[p].type == TK_NEG) {
        uint32_t val = eval(p + 1, q, success);
        if (!*success) return 0;
        return -(int32_t)val;  // 转为有符号数取负
      }
      if (tokens[p].type == TK_DEREF) {
        uint32_t addr = eval(p + 1, q, success);
        if (!*success) return 0;
        return vaddr_read(addr, 4);  // 解引用，读取4字节
      }
      // 没找到主运算符，表达式不合法
      *success = false;
      return 0;
    }

    int op_type = tokens[op_pos].type;

    // 双目运算符
    // 递归求左右子表达式
    uint32_t val1 = eval(p, op_pos - 1, success);
    if (!*success) return 0;

    uint32_t val2 = eval(op_pos + 1, q, success);
    if (!*success) return 0;

    // 根据主运算符计算结果
    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/':
        if (val2 == 0) {
          printf("Error: division by zero\n");
          *success = false;
          return 0;
        }
        return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NE: return val1 != val2;
      case TK_AND: return val1 && val2;
      default:
        *success = false;
        return 0;
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  uint32_t result = eval(0, nr_token - 1, success);

  return result;
}
