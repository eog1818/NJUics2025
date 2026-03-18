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



static int32_t buf_index = 0;
static void gen(char str){
  buf[buf_index] = str;
  buf_index++;
  buf[buf_index] = '\0';
}

static uint32_t choose(uint32_t N){
  //uint32_t num1=N;
  //srand(time(NULL));
  //uint32_t random_range_num1 = rand() % num1;
  double num1 = (double) rand() / ((double) RAND_MAX+1.0); // [0-1)
  uint32_t num2 =  (uint32_t) (num1 * N); // [0,N)
  return num2;
  
}

static void gen_space(){
  //srand(time(NULL));
  uint32_t random_range_num = choose(3);// rand() % 5;
  //char *dest_space = (char*)malloc(random_range_num*sizeof(char));
  for (uint32_t i=0;i<random_range_num;i++){
      gen(' ');
  }
  //dest_space[random_range_num]='\0';
  //strcat(buf, dest_space); 
  //free(dest_space); */
  //return NULL;
   //char str1[3]="   ";
   //snprintf(str1, sizeof(str1), "%s", "     ");
   //strcat(buf, "  "); 
}
static void gen_num(){
  
  uint32_t len=2;
  uint32_t digit1=choose(10);
  if (digit1==0){
    digit1++;
  }
  gen((char) (digit1+'0'));

  uint32_t digit2=choose(10);
  gen((char) (digit2+'0'));
}

static void gen_rand_op(){

  switch(choose(4))
  {
    case 0:  
      gen('+'); break;
    case 1:  
      gen('-'); break;
    case 2:  
      gen('*'); break;
    default: 
      gen('/'); break;

  }

}
static uint32_t gen_rand_expr() {
  uint32_t choosenum=choose(3);
  
  switch(choosenum){
	  //case 0: gen_space(); gen_num(); gen_space(); break;
    //case 1: gen('('); gen_space(); gen_rand_expr(); gen_space();gen(')'); break;
    //default: gen_rand_expr(); gen_space(); gen_rand_op(); gen_space(); gen_rand_expr(); break;
    case 0: gen_num();;break;
	  case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
    
	}
}
static uint32_t gen_rand_expr_n(uint32_t n) {
  //buf[0] = '\0';
  //switch (1) {
  uint32_t choosenum=choose(3);
  if (n >= 3){
    choosenum = 0;
  }
  switch(choosenum){
	  //case 0: gen_space(); gen_num(); gen_space(); break;
    case 0: gen_space();gen_num();gen('u');gen_space();break;
	  case 1: gen('('); gen_space(); gen_rand_expr_n(1); gen_space();gen(')'); break;
    //case 2: gen_num();gen_rand_op();gen_num();break;
	  default: gen_rand_expr_n(1); gen_space(); gen_rand_op(); gen_space(); gen_rand_expr_n(1); break;
    
	}
  return choosenum;

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
    
    buf_index = 0;
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
    int status;
    ret = fscanf(fp, "%d", &result);
    status = pclose(fp);

    if (WIFEXITED(status)){
      /*indicates a divide-by-zero operation*/
      if (WEXITSTATUS(status) == 136){
        continue;
      }
    }

    printf("%u %s\n", result, buf);
  }
  return 0;
}
