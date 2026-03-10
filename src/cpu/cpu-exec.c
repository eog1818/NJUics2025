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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include <stdint.h>
#include <inttypes.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 20

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
  printf("step in trace and difftest\n");
  printf("CONFIG_ITRACE_COND is %s\n",CONFIG_ITRACE_COND);
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { 
    log_write("%s\n", _this->logbuf); 
    printf("logbuf is %s\n",_this->logbuf);
  }
#endif
  printf("g_print_step is %d\n", g_print_step);
  printf("CONFIG_ITRACE is %d\n",CONFIG_ITRACE);
  //printf("CONFIG_DIFFTEST is %s\n",CONFIG_DIFFTEST);
  if (g_print_step) { 
    printf("step g print step, %d\n", g_print_step);
    IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
    IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
}

static void exec_once(Decode *s, vaddr_t pc) {
  printf("step exec once\n");
  s->pc = pc;
  s->snpc = pc;
  printf("step going to isa exec once\n");
  isa_exec_once(s);
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst;
#ifdef CONFIG_ISA_x86
  for (i = 0; i < ilen; i ++) {
#else
  for (i = ilen - 1; i >= 0; i --) {
#endif
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  printf("step going to memset\n");
  memset(p, ' ', space_len);
  p += space_len;
  printf("step going to disassemble\n");
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  printf("step going to disassemble2\n");
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst, ilen);
#endif
}

static void execute(uint64_t n) {
  printf("step execute\n");
  Decode s;
  for (;n > 0; n --) {
    //printf("n is %ld\n", n);
    printf("n is %" PRIu64 "\n", n);
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    printf("step going to trace and difftest\n");
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic() {
  printf("step statistic\n");
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  printf("step in assert fail msg\n");
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  printf("step in cpu exec\n");
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT: case NEMU_QUIT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }
  printf("step going to get time\n");
  uint64_t timer_start = get_time();
  printf("step going to execute\n");
  execute(n);

  printf("step going to get time2 \n");
  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {

    case NEMU_RUNNING: 
      printf("step NEMU_RUNNING to NEMU_STOP\n");
      nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      printf("step NEMU_END or NEMU_ABORT\n");
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: 
      printf("step NEMU_QUIT\n");
      printf("step going to statistic\n");
      statistic();
  }
}
