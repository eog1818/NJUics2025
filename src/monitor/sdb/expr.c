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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>


int eval(int p,  int q);
bool check_parentheses( int p, int q);
int position_primary_op(int p, int q);
bool is_op_inside_parentheses(char op_to_find, int p, int q);

enum {
  //TK_NOTYPE = 256, TK_EQ,TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVID, TK_DECNUM, TK_LeftPrent, TK_RightPrent 
  TK_NOTYPE = 256, TK_EQ,TK_PLUS=258, TK_MINUS=259, TK_MULTIPLY=260, TK_DIVID=261, TK_DECNUM=262, TK_LeftPrent=263, TK_RightPrent=264 ,
  
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", TK_MINUS}, // minus
  {"\\*", TK_MULTIPLY}, // multiple
  {"\\/", TK_DIVID}, // divid
  {"[[:digit:]]+", TK_DECNUM}, // decimal number "\\d+" is not working
  {"\\(", TK_LeftPrent}, // left p
  {"\\)", TK_RightPrent}, // right p
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  printf("step init regex\n");
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

//static Token tokens[32] __attribute__((used)) = {};
//static int nr_token __attribute__((used))  = 0;
static Token tokens[32] = {};
static int nr_token  = 0;
//for (int i = 0; i < 32; i++) {
//    strcpy(tokens[i].str, "\0");
//    tokens[i].type=0;
//}

static bool make_token(char *e) {
  //e= " + 5+4*30/200-1";
  printf("step is make token\n");
  int position = 0;
  int i;
  regmatch_t pmatch;

  //nr_token = 0;

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

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_EQ:
            break;
          
            
          default: 
           //if (rules[i].token_type != TK_NOTYPE){
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start + 0, substr_len);
            nr_token=nr_token+1;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int eval(int p,  int q){
  int op, val1, val2;
  int num;
  if (p>q){
    printf("bad expression");
  }
  else if (p == q) {
    printf("single token");
    sscanf(tokens[p].str, "%d", &num);
    return num;   
  } 
  else if (check_parentheses(p, q) == true){

    return eval(p+1, q-1);
  }
  else {
    op = position_primary_op( p, q);
    val1=eval(p,op-1);
    val2=eval( op+1,q);

    switch(tokens[op].type){
      case TK_PLUS : return val1+val2;
      case TK_MINUS : return val1-val2;
      case TK_MULTIPLY : return val1*val2;
      case TK_DIVID : return val1/val2;
      default: 
      assert(0);
      
    }
    
  }
  return 0;
}

bool check_parentheses( int p, int q){
  int num_pr=0;
  if (tokens[p].type!=TK_LeftPrent || tokens[q].type!=TK_RightPrent){
    //assert(0);
    printf("false, the whole expression is not surrounded by a matched pair of parenthesis.\n");
    return false;
  }
  for (int i =p; i<=q; i++)
  {
    if (tokens[i].type == TK_LeftPrent){
      num_pr++;

    }else if(tokens[i].type == TK_RightPrent){
      num_pr--;
    }
    if (num_pr<0){
      printf("false, bad expression.\n");
      return false;
      //assert(0);
    }else if (num_pr==0 && i<q){
      printf("false, the leftmost '(' and the rightmost ')' are not matched\n");
      return false;
    }
  }

  if(num_pr==0){
      printf("true in check parentheses\n");
      return true;
  }
  else{
    printf("false in check parentheses\n");
    return false;
    //continue;
  }

}

bool is_op_inside_parentheses(char op_to_find, int p, int q){
  int paren_count=0;
  //int len =q-p+1;
  for (int i=p;i<q;i++){
    if( tokens[i].str[0] == '(' ){
      paren_count++;
    }else if (tokens[i].str[0] == ')'){
      paren_count--;
    }else if(tokens[i].str[0] == op_to_find ){
      if (paren_count > 0)
      {
        printf("op to find is in the paretheses\n");
        return true;
      }
    }
   
  }
    printf("op to find is not in the paretheses\n");
    return false;
}

int position_primary_op(int p, int q){

  int len = q-p+1;
  int status_op[len];
  
  int num_plus_minus=0, num_mult_divid=0;

  for(int i=0;i<len;i++){
    status_op[i]=0;
  } 
  for (int i=0;i<len;i++){
    if(tokens[p+i].str[0]== '+' || tokens[p+i].str[0] == '-'){
      status_op[i] = 1;
      num_plus_minus++;
    }

    if( tokens[p+i].str[0] == '*' ||tokens[p+i].str[0] == '/' ){
      status_op[i] = 2;
      num_mult_divid++;
    }
  }
  
  if(num_plus_minus == 0 && num_mult_divid ==0){
    printf("no op found, so no position primary found");
    return p;
  }


  for (int i=0;i<len;i++){
    if (status_op[i] == 1){
      if((is_op_inside_parentheses(tokens[p+i].str[0], p, q) ) == true) {
        status_op[i] = 0;
        num_plus_minus--;
      }
      
    }

    if (status_op[i] == 2){
      if((is_op_inside_parentheses(tokens[p+i].str[0], p, q) ) == true) {
        status_op[i] = 0;
        num_mult_divid--;
      }
    }
    
  }

  if (num_plus_minus!=0 && num_mult_divid!=0){
    for (int i=len;i>0;i--){
      if (status_op[i] == 1){
        return p+i;
      }
    }
  }
  if (num_plus_minus!=0 && num_mult_divid==0){
    for (int i=len;i>0;i--){
      if (status_op[i] == 1){
        return p+i;
      }
    }
  }

  if (num_plus_minus ==0 && num_mult_divid!=0){
    for (int i=len;i>0;i--){
      if (status_op[i] == 2){
        return p+i;
      }
    }
  }
  return 0;
} 


word_t expr(char *e, bool *success) {
  word_t result;
  printf("step is expr\n");
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  printf("step is going to TODO eval\n");
  //TODO();
  result = (word_t) eval(0,  nr_token-1);
  printf("results is %" PRIu32 "\n",result);
  //printf("results is %d\n",result);
  
  return result;//0-10

  //return 0;

}

