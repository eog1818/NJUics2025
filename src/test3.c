
#ifndef __test1__
#define __test1__

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "debug.h"
#endif

#define NR_REGEX ARRLEN(rules)
// calculate the length of an array
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)
#define CONST_Token_SIZE 100

#define str_temp(x) #x
#define STR(x) str_temp(x)
typedef __uint8_t uint8_t;

//static int cmd_help(char *args);
static int cmd_Si(char *args){
	return 0;
}

static int cmd_c(char *args) {
//   cpu_exec(-1);
   return 0;
}
 
 
static int cmd_q(char *args) {
   return -1;
 }
 
static int cmd_help(char *args);
 
//static int cmd_Si(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si [N]", "Execute N step and stop, default N = 1 ", cmd_Si }
  /* TODO: Add more commands */

};


static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    //for (i = 0; i < NR_CMD; i ++) {
    for (i = 0; i < 2; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for (i = 0; i < NR_CMD; i ++) {
		if (strcmp(arg, cmd_table[i].name) == 0) {
	        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    	    return 0;
			}

		}
    printf("Unknown command '%s'\n", arg);
	}
  return 0;
}

void cpu_exec(uint8_t n) {
	for (;n > 0; n --) {

		printf("n is %d\n", n);
		
		//execute(n);
	}

}



	
enum {
  TK_NOTYPE = 256, TK_EQ,TK_PLUS=258, TK_MINUS=259, TK_MULTIPLY=260, TK_DIVID=261, TK_DECNUM=262, TK_LeftPrent=263, TK_RightPrent=264 ,
  //TK_DECNUM = 100,
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
  {"[[:digit:]]+", TK_DECNUM}, // decimal number
  {"\\(", TK_LeftPrent}, // left p
  {"\\)", TK_RightPrent}, // right p
  

};

int op, val1, val2;
int eval(Token *tokens, int p,  int q){
  if (p>q){
    printf("bad expression");
  }
  else if (p == q) {
    printf("single token");
    return p;   
  } 
  else if (check_parentheses(tokens, p, q) == true){

    return eval(p+1, q-1);
  }
  else {
    op = posision_primary_op(tokens, p, q);
    val1=eval(p,op-1);
    val2=eval(op+1,q);

    switch(op_type){
      case TK_PLUS : return val1+val2;
      case TK_MINUS : return val1-val2;
      case TK_MULTIPLY : return val1*val2;
      case TK_DIVID : return val1/val2;
      default: assert(0);
    }
    
  }
}

bool check_parentheses(Token *tokens, int p, int q){
  int num_pr=0;
  if (tokens[p].type!=TK_LeftPrent || tokens[q].type!=TK_RightPrent){
    //assert(0);
    printf("false, the whole expression is not surrounded by a matched pair of parenthesis.\n");
  }
  for (int i =p; i<=q; i++)
  {
    if (tokens[p].type == TK_LeftPrent){
      num_pr++;

    }else if(tokens[p].type == TK_RightPrent){
      num_pr--;
    }
    if (num_pr<0){
      printf("false, bad expression.\n");
      //assert(0);
    }else if (num_pr==0){
      printf("falsse, the leftmost '(' and the rightmost ')' are not matched\n");
    }
  }
  if(num_pr==0){
      printf("true in check parentheses\n");
  }

}

int posision_primary_op(Token *tokens, int p, int q){
  int i=0;
  while (tokens[i].str != "\0"){
    

    i=i+1;
  }
  //tokens[i].str, tokens[i].type);
  //struct token *tokens; 
  //int tokensize =CONST_Token_SIZE;
  //tokens = (struct token*)malloc( tokensize * sizeof(struct token));
  

}

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
	  printf("i = %d, ret = %d\n",i, ret);

    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      printf("regex compilation failed:\n");
      //panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token{

  int type;
  char str[32];
} Token;



static bool make_token(char *e) {
  printf("step is make token\n");
  //printf("e is %s\n", e);
  int position = 0;
  int i;
  regmatch_t pmatch;

  //static Token tokens[32] __attribute__((used)) = {};
  //static int nr_token __attribute__((used))  = 0;
  //struct token *tokens; 
  Token *tokens;
  int tokensize =CONST_Token_SIZE;
  tokens = (struct token*)malloc( tokensize * sizeof(Token));
  
//int size  = 100;
  if(tokens == NULL){
    perror("Memory allocation failed!");
    return EXIT_FAILURE;
  }
  
  for (int i = 0; i < CONST_Token_SIZE; i++) {
    strcpy(tokens[i].str, "\0");
    tokens[i].type=0;
  }

  int nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    //int jj=0;
    for (int i = 0; i < NR_REGEX; i ++) {
      
      //int ii = regexec(&re[i], e + position, 1, &pmatch, 0);
      //jj = jj+1;
      //printf("jj = %d", jj);
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
      //if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        
        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);

        //strcpy(tokens[nr_token].str, rules[i].regex);
        if (rules[i].token_type != TK_NOTYPE){
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start + 0, substr_len);
          nr_token=nr_token+1;
        }

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        

        switch (rules[i].token_type) {
          case TK_PLUS: case TK_MINUS: case TK_MULTIPLE: case TK_DIVID:
            //eval();


          default: 
            //TODO();
            printf("TODO() %s, %d\n", tokens[nr_token].str, tokens[nr_token].type);
		        //printf("TODO() - %d\n", jj);
            //eval();
        }

        break;
      }
    }
   
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
    

  }

  //eval();

  for(int i;i<20;i++){
      printf("tokens are %s, %d\n", tokens[i].str, tokens[i].type);
    }
  return true;
}
int main(){
	char *e = "(4 +3*(2- 1))";
	init_regex();
	make_token(e);
  //printf("%s\n", e);


	return 0;
}
