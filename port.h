#ifndef _port_h
#define _port_h

#include "system.h"
#include "altera_avalon_uart_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "serial.h"
#include "sys/alt_irq.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "serial.h"

volatile int led_status;

void flash_timer_init ();

void isr_uart_rs232(void* context);
void isr_flash_timer(void* context);

#endif
