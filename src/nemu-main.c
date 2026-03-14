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
#ifndef __test1__
#define __test1__

#include <common.h>

#endif

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  printf("step nemu main\n");
  /* Initialize the monitor. */
  
#ifdef CONFIG_TARGET_AM
  //printf("CONFIG_TARGET_AM is %d\n",CONFIG_TARGET_AM);
  printf("step is going to am_init_monitor");
  am_init_monitor();
#else
  printf("step is going to init monitor\n");
  init_monitor(argc, argv);
  
#endif

  /* Start engine. */
  printf("step is going to egnine start\n");
  engine_start();

  printf("step is going to is exit status bad\n");
  return is_exit_status_bad();
}
