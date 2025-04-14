/*
 * Copyright: Olivier Gruber (olivier dot gruber at acm dot org)
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#include "main.h"
#include "uart.h"
#include "isr.h"

extern uint32_t irq_stack_top;
extern uint32_t stack_top;

// Called whenever there are available bytes
void read_listener(void *addr) {
  struct cookie *cookie = addr;
  uint8_t code;
  while (!cookie->processing && uart_receive(cookie->uartno,&code)) {
    cookie->line[cookie->head++]=(char)code;
    cookie->processing = (code == '\n');
    write_amap(cookie);
  }
  bool_t dropped = false;
  while (cookie->processing && uart_receive(cookie->uartno,&code)){
    dropped = true;
  }
  if (dropped){
    //beep(); // signal dropped bytes...
  }
}

 // Called when there are available bytes
void write_listener(void *addr) {
  struct cookie *cookie = addr;
  write_amap(cookie);
}

void write_amap(struct cookie *cookie) {
  while (cookie->tail < cookie->head) {
    uint8_t code = cookie->line[cookie->tail];
    // if (!uart_write(cookie->uartno,code)){
    //   return;
    // }
    cookie->tail++;
    if (code == '\n') {
      // shell(cookie->line,cookie->head);
      cookie->tail= cookie->head = 0;
      cookie->processing=false;
    }
  }
}

void check_stacks() {
  void *memsize = (void*)MEMORY;
  void *addr;
  addr = &stack_top;
  if (addr >= memsize)
    panic();
/*
  addr = &irq_stack_top;
  if (addr >= memsize)
    panic();
*/
}

/**
 * This is the C entry point,
 * upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */
// void _start(void) {
//   char c;
//   check_stacks();
//   uarts_init();
//   uart_enable(UART0);
//   for (;;) {
//     uart_receive(UART0, &c);
//     uart_send(UART0, c);
//   }
// }

void _start() {
  char c;
  check_stacks();
  uarts_init();
  uart_enable(UART0);
  vic_setup_irqs();
  //vic_enable_irq(?, ?, ?);
  core_enable_irqs();
  for (;;) {
    core_halt();
  }
}

void panic() {
  for(;;)
    ;
}
