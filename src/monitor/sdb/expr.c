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
#include <stdbool.h>

#define NR_REGEX ARRLEN(rules)
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)
#define CONST_Token_SIZE 100
#define MAX_LINE_LENGTH 1024
//#define true 1
//#define false 0

bool check_parentheses( int p, int q);
int position_primary_op(int p, int q);
int position_primary_op_old(int p, int q);
bool Is_minus_op_negative_op(int p, int i);
word_t eval(int p,  int q, _Bool eEvalSuccess);
bool is_op_inside_parentheses(char op_to_find, int p, int q);

#define STACK_MAX_SIZE 1000

typedef struct {
char token_str;
int token_type; // Array to store stack elements
int token_position;
 // Index of the top element
} Stack_node;

typedef struct {
Stack_node *Stack_node; // Array to store stack elements
int top; // Index of the top element
} Stack;

// Initialize the stack
void initialize(Stack *stack) {
  //Stack* stack = (Stack*)malloc(sizeof(Stack));
  //if (!stack) return NULL;
  stack->top = -1;
  stack->Stack_node=(Stack_node*)malloc(STACK_MAX_SIZE*sizeof(Stack_node));
  //stack = (Stack*)malloc(sizeof(Stack));
}

// Check if the stack is empty
bool isEmpty(Stack *stack) {

return stack->top == -1;
}

// Check if the stack is full
bool isFull(Stack *stack) {
return stack->top == STACK_MAX_SIZE - 1;
}

// Push an element onto the stack
void push(Stack *stack, Stack_node *value) {
  if (isFull(stack)) {
  printf("Stack Overflow\n");
  return;
  }
  
  stack->top++;
  //stack->Stack_node[++stack->top] = value;
  stack->Stack_node[stack->top].token_position=value->token_position;
  stack->Stack_node[stack->top].token_type=value->token_type;
  stack->Stack_node[stack->top].token_str=value->token_str;

//[stack->top].token_position = value.token_position;

//printf("Pushed %d onto the stack\n", value);
}

// Pop an element from the stack
Stack_node pop(Stack *stack) {
  if (isEmpty(stack)) {
    printf("Stack Underflow\n");
    Stack_node *a = (Stack_node*)malloc(sizeof(Stack_node));
    a->token_position=-1;
    a->token_str='\0';
    a->token_type=-1;
    return *a;
  }
//return stack->arr[stack->top--];
  return stack->Stack_node[stack->top--];
}

// Peek the top element of the stack
Stack_node peek(Stack *stack) {
  if (isEmpty(stack)) {
    printf("Stack is empty\n");
    Stack_node *a = (Stack_node*)malloc(sizeof(Stack_node));
    a->token_position=-1;
    a->token_str='\0';
    a->token_type=-1;
    
    return *a;
  }
  return stack->Stack_node[stack->top];
}
//////////////////
enum {
  //TK_NOTYPE = 256, TK_EQ,TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVID, TK_DECNUM, TK_LeftPrent, TK_RightPrent 
  
  TK_NOTYPE = 256, TK_EQ,TK_PLUS=258, TK_MINUS=259, TK_MULTIPLY=260, TK_DIVID=261, TK_DECNUM=262, TK_LeftPrent=263, TK_RightPrent=264 ,TK_Negative=265 ,
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

//#define NR_REGEX ARRLEN(rules)

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
  char str[100];
} Token;

//static Token tokens[32] __attribute__((used)) = {};
//static int nr_token __attribute__((used))  = 0;
static Token tokens[5000] = {};
static int nr_token  = 0;
//for (int i = 0; i < 32; i++) {
//    strcpy(tokens[i].str, "\0");
//    tokens[i].type=0;
//}

static bool make_token(char *e) {
  //e= " + 5+4*30/200-1";
  nr_token = 0;
  printf("step is make token\n");
  int position = 0;
  int i;
  regmatch_t pmatch;

  //nr_token = 0;

  while (e[position] != '\0') {
    // Try all rules one by one. 
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        position += substr_len;
        
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        Log("match rules [%d] = \"%s\" at position %d with len %d: %.*s", 
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        // TODO: Now a new token is recognized with rules[i]. Add codes
        // to record the token in the array `tokens'. For certain types
        // of tokens, some extra actions should be performed.
        //

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_EQ:
            break;
          case TK_DECNUM:
            //Assert(nr_token<65535, "The tokesn array has insufficient storge spece.");
            //Assert(substr_len<32, "token is too long");
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].str[substr_len]='\0';
            nr_token++;
            break;
          
            
          default: 
           //if (rules[i].token_type != TK_NOTYPE){
            //Assert(nr_token<65535, "The tokens array has insufficient storage space. ");
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
/*
static bool make_token_old(char *e) {
  //e= " + 5+4*30/200-1";
  printf("step is make token\n");
  nr_token = 0;
  int position = 0;
  int i;
  regmatch_t pmatch;

  //nr_token = 0;

  while (e[position] != '\0') {
    
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        //Log("match rules [%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position,
        //  substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_EQ:
            break;
          case TK_DECNUM:
            Assert(nr_token<65535, "The tokesn array has insufficient storge spece.");
            Assert(substr_len<32, "token is too long");
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].str[substr_len]='\0';
            nr_token++;
            break;
          
            
          default: 
           //if (rules[i].token_type != TK_NOTYPE){
            Assert(nr_token<65535, "The tokens array has insufficient storage space. ");
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
}*/
/*
word_t eval_old(int p,  int q){
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
      //Assert(0, 0);
      
    }
    
  }
  return 0;
}
*/

word_t eval(int p,  int q, _Bool eEvalSuccess){
  int op; 
  word_t val1, val2;
  word_t num;
  int num1;
  //int result;
  if (p>q){
    printf("bad expression");
  }
  else if (p == q) {
    printf("single token");
    sscanf(tokens[p].str, "%d", &num);
    //*eEvalSuccess = true;
    return num;   
  } 
  else if (check_parentheses(p, q) == true){

    return eval(p+1, q-1, eEvalSuccess);
  }else if((q-p)==1){
    if (tokens[p].type==TK_Negative){
      printf("negatibe token and single number token");
      sscanf(tokens[q].str, "%d", &num1);
      //printf("The value is: %" PRIu32 "\n", value);
      //result = sscanf(tokens[q].str, %" PRIu32 ", &num);
      num = (uint32_t) ((-1) * num1);
      //*eEvalSuccess = true;
      return num;   
    }
  } else {
    op = position_primary_op( p, q);
    val1=eval(p,op-1, eEvalSuccess);
    val2=eval( op+1,q, eEvalSuccess);

    switch(tokens[op].type){
      case TK_PLUS : 
        return val1+val2;
      case TK_MINUS : 
        return val1-val2;
      case TK_MULTIPLY : 
        return val1*val2;
      case TK_DIVID : 
        return val1/val2;
      default: 
        printf("the position primary must be +=*/.\n");
        assert(0);
        return 0;
    }
    
  }
  return 0;
}
/*
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
  */


bool check_parentheses( int p, int q){
  int num_pr=0;
  if (tokens[p].type!=TK_LeftPrent || tokens[q].type!=TK_RightPrent){
    //assert(0);
    printf("false, the whole expression is not surrounded by a matched pair of parenthesis.\n");//"4 + 3 * (2 - 1)" 
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
      printf("false, bad expression.\n"); //"(4 + 3)) * ((2 - 1)"
      return false;
      //assert(0);
    }else if (num_pr==0 && i<q){ 
      printf("false, the leftmost '(' and the rightmost ')' are not matched\n");//"(4 + 3) * (2 - 1)"
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

/*
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
    */

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
        printf("op to be found is in the paretheses\n");
        return true;
      }
    }
   
  }
    printf("op to be found is not in the paretheses\n");
    return false;
}

bool Is_minus_op_negative_op(int p, int i){
  //assert(tokens[p+i]->type==TK_MINUS);
  
  if (tokens[p+i].type == TK_MINUS && tokens[p+i-1].type != TK_RightPrent && tokens[p+i-1].type!=TK_DECNUM){
    
    return true;
  } else{
    return false;
  }
}
/*
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
*/

int position_primary_op(int p, int q){
  int len = q-p+1;

  //int status_op[len];
  //int num_plus_minus=0, num_mult_divid=0;
  int Primary_position=p;

  Stack *s;
  Stack_node *a1,*a2;
  s = (Stack*)malloc(sizeof(Stack));
  a1 = (Stack_node*)malloc(sizeof(Stack_node));
  a2 = (Stack_node*)malloc(sizeof(Stack_node));
  a1->token_position=-1;
  a1->token_str='\0';
  a1->token_type=-1;
  
  a2->token_position=-1;
  a2->token_str='\0';
  a2->token_type=-1;
  
  initialize(s);
/*
  for(int i=0;i<len;i++){
    if (tokens[p+i].type == TK_DECNUM)
      //str[0] == '+')
      status_op[i]=0;
      else if (tokens[p+i].type == TK_PLUS){
        status_op[i]=1;  
      }
      else if (tokens[p+i].type == TK_MINUS){
        if(Is_minus_op_negative_op(p, i)==true){
          tokens[p+i].type = TK_Negative;
          status_op[i]=3;
        } else {
          status_op[i]=1;
        }
      } else if (tokens[p+i].type == TK_MULTIPLY){
        status_op[i]=2;  
      } else if(tokens[p+i].type == TK_DIVID){
        status_op[i]=2;  
      } 
      else {
        status_op[i]=0;  
        //assert(0);
      }
    }  */
  for(int i=0;i<len;i++){
    if(tokens[p+i].type == TK_DECNUM || tokens[p+i].type == TK_Negative){
      continue;
    } else if ((tokens[p+i].type == TK_PLUS))
    {
      a1->token_type=TK_PLUS;
      a1->token_str='+';
      a1->token_position=p+i;
      push(s, a1);
    }else if ((tokens[p+i].type == TK_MINUS))
    {
      if (tokens[p+i].type != TK_Negative){
        a1->token_type=TK_MINUS;  
        a1->token_str='-';
        a1->token_position=p+i;
        push(s, a1);
      }
    }else if ((tokens[p+i].type == TK_MULTIPLY))
    {

      a1->token_type=TK_MULTIPLY;  
      a1->token_str='*';
      a1->token_position=p+i;
      push(s, a1);
    }else if ((tokens[p+i].type == TK_DIVID))
    {
      a1->token_type=TK_DIVID;  
      a1->token_str='/';
      a1->token_position=p+i;
      push(s, a1);
    }else if ((tokens[p+i].type == TK_LeftPrent))
    {
      a1->token_type=TK_LeftPrent;  
      a1->token_str='(';
      a1->token_position=p+i;
      push(s, a1);
    }else if ((tokens[p+i].type == TK_RightPrent))
    {
      //*a2 = pop(s);
      while (s->top>=0 ) {
        *a2 = pop(s);
        if (a2->token_type == TK_LeftPrent){
          break;
        }
        
      }
      //break;
      
    }else{
      printf("can't find primary position of op.\n");
      assert(0);
    }
  }
  *a2 = pop(s);
  if(a2->token_type == TK_PLUS || a2->token_type == TK_MINUS){
    Primary_position = a2->token_position;
    return Primary_position;
  }else if (a2->token_type == TK_MULTIPLY || a2->token_type == TK_DIVID){
    Primary_position = a2->token_position;
    while (s->top>=0 ) {
        *a2 = pop(s);
        if(a2->token_type == TK_PLUS || a2->token_type == TK_MINUS){
          Primary_position = a2->token_position;
          return Primary_position;
        }else if(a2->token_type == TK_MULTIPLY || a2->token_type == TK_DIVID){
          continue;
        }

    }
    
  }
  return Primary_position;


  //return -1;
}
/*
word_t expr(char *e, bool *success) {
  word_t result;
  printf("step is expr\n");
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i=0;i<nr_token;i++){
    printf("token is %d %s\n", tokens[i].type, tokens[i].str);
  }
  
  // TODO: Insert codes to evaluate the expression. 
  printf("step is going to TODO eval\n");
  //TODO();
  result = (word_t) eval(0,  nr_token-1);
  printf("results is %" PRIu32 "\n",result);
  //printf("results is %d\n",result);
  
  return result;//0-10

  //return 0;

}
*/

word_t expr(char *e, bool *success) {
  word_t result;
  //bool *evalSuccess= true;
  bool evalSuccess= true;
  printf("step is expr\n");
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // TODO: Insert codes to evaluate the expression. 
  printf("step is going to TODO eval\n");
  //TODO();
  result = (word_t) eval(0,  nr_token-1, evalSuccess);
  if (evalSuccess == true){
    printf("results is %" PRIu32 "\n",result);
    return result;//0-10
  }
  //printf("results is %d\n",result);

  return 0;

}