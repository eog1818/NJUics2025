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
#ifndef YOUR_HEADER_H
#define YOUR_HEADER_H

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <utils.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <common.h>
#include <inttypes.h>
#include <stdbool.h>

#endif

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  printf("step rl gets\n");
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
  //return -1;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
  
}

static int cmd_help(char *args);

static int cmd_Si(char *args);
static int cmd_info_r(char *args);
static int cmd_info_w(char *args);
static int cmd_print_mem(char *args);
static int cmd_calc_expr(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Si [N], Execute N step and stop, default N = 1 ", cmd_Si },
  { "ir", "info r, print the status of registers", cmd_info_r},
  { "iw", "info w, print the staus of watches", cmd_info_w },
  { "x", "x N EXPR, scan and print the memory, ", cmd_print_mem },
  { "p", "p EXPR, calc the EXPR value, ", cmd_calc_expr },
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)


static int cmd_calc_expr(char *args){
  
  if (args == NULL){
    return 0;
  }
  //char *e = "(4 +3)*(2- 1)";
  word_t result;
	init_regex();
	//make_token(e);
  //printf("%s\n", e);
  bool *success =NULL;
  result = expr(args, success);
  printf("results is %" PRIu32 "\n",result);
  printf("results is 0x%" PRIx32 "\n",result);

  return 0;
}


static int cmd_Si(char *args){
    printf("step in cmd Si\n");
    //const char delim[]=" ";
    //char *input = readline();
    printf("args is %s\n", args);
    int n;
    int num=0;
    if (args == NULL){
      n = 1;
      //cpu_exec(n);
      //num = 0;
    }
    else {      
      //n = atoi(args);
      //cpu_exec(n);
      num = sscanf(args, "%d", &n);
    }
    cpu_exec(n);
    printf("num is %d\n", num);
  return 0;
}

static int cmd_info_r(char *args){
  isa_reg_display();
  return 0;
}

static int cmd_info_w(char *args){
  isa_watchpoint_display();
  return 0;
}


static int cmd_print_mem(char *args){
  printf("step in cmd print mem\n");
  printf("args is %s\n", args);
  word_t start_addr = 0x80000000;//word_t is unit32_t in the 32bit ISA
  int len = 30;// it is the number of every 4 bytes (32 bits)
  for (int i=0;i<(len/4);i++){
    printf("|0x%-10x|", start_addr+i*16);
      word_t re_addr1 = vaddr_read(start_addr+i*16+0*4, 4);
      word_t re_addr2 = vaddr_read(start_addr+i*16+1*4, 4);
      word_t re_addr3 = vaddr_read(start_addr+i*16+2*4, 4);
      word_t re_addr4 = vaddr_read(start_addr+i*16+3*4, 4);
      printf("|0x%-10x|", re_addr1);
      printf("|0x%-10x|", re_addr2);
      printf("|0x%-10x|", re_addr3);
      printf("|0x%-10x|\n", re_addr4);
  }
  return 0; 
}
/*
static int cmd_print_mem(char *args){
  printf("step in cmd print mem\n");
  printf("args is %s\n", args);
  word_t start_addr = 0x80000000;//word_t is unit32_t in the 32bit ISA
  //word_t start_addr = 0x100000;//word_t is unit32_t in the 32bit ISA
  //char *str_num_addr = strtok(args, " ");
  int len = 30;// it is the number of every 4 bytes (32 bits)
  //sscanf();  
  //word_t paddr_read(paddr_t addr, int len) {
  //word_t vaddr_read(vaddr_t addr, int len) {
  //word_t vaddr_read(vaddr_t addr, int len);

  //printf("|%-*s|",10, "pc");    
  //printf("|0x%-10x|", cpu.pc);
  //printf("|%-16u|\n", cpu.pc);
  for (int i=0;i<(len/4);i++){
    printf("|0x%-10x|", start_addr+i*16);
    //for (int j=0;j<3;j++){
      //word_t re_addr = vaddr_read(start_addr+i*4+j, 4);//16 bytes, such as 0x555555558389, 0x555555558399, 
    //printf("|0x%-10x|\n", vaddr_read(start_addr, len));
    //if (start_addr+i*16+0*4 <= len){
      word_t re_addr1 = vaddr_read(start_addr+i*16+0*4, 4);
      word_t re_addr2 = vaddr_read(start_addr+i*16+1*4, 4);
      word_t re_addr3 = vaddr_read(start_addr+i*16+2*4, 4);
      word_t re_addr4 = vaddr_read(start_addr+i*16+3*4, 4);
      //char buffer1[12], buffer2[12], buffer3[12], buffer4[12];
      //snprintf(buffer1, sizeof(buffer1), "%" PRIu32, re_addr1);
      //snprintf(buffer2, sizeof(buffer2), "%" PRIu32, re_addr2);
      //snprintf(buffer3, sizeof(buffer3), "%" PRIu32, re_addr3);
      //snprintf(buffer4, sizeof(buffer4), "%" PRIu32, re_addr4);

      printf("|0x%-10x|", re_addr1);
      //printf("|%-16d|",   re_addr1);
      printf("|0x%-10x|", re_addr2);
      //printf("|%-12s|", buffer2);
      //printf("|%-16d|",   vaddr_read(start_addr+i*4+1, 4));
      printf("|0x%-10x|", re_addr3);
      //printf("|%-12s|", buffer3);
      //printf("|%-16d|",   vaddr_read(start_addr+i*4+2, 4));
      printf("|0x%-10x|\n", re_addr4);
      //printf("|%-12s|\n", buffer4);
      //printf("|%-16d|\n", vaddr_read(start_addr+i*4+3, 4));
    //snprintf(buffer, sizeof(buffer), "%" PRIu32, value);
  }
  return 0; 
}
*/
static int cmd_help(char *args) {
  printf("step cmd help\n");
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
    //for (i = 0; i < 2; i ++) {
      printf("inside arg==NULL: %d ", i);
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("inside arg!=NULL: %d ", i);
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  printf("step sdb mainloop\n");
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
   printf("step is going to rl_gets\n");
   int ic =0;
  for (char *str; (str = rl_gets()) != NULL; ) {
    
    printf("still in the mainloop, number is %d\n", ic);
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }
  printf("cmd: %s \n", cmd);
  printf("args: %s \n", args);
#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      printf("inside mainloop: %d \n", i);
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { 
          return; 
          //break;
        }
        //return;
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  
    ic++;
  }
  
}

void init_sdb() {
  printf("step init sdb\n");
  /* Compile the regular expressions. */
  printf("step is going to init regex\n");
  init_regex();

  /* Initialize the watchpoint pool. */
  printf("step is going to wp pool\n");
  init_wp_pool();
}
