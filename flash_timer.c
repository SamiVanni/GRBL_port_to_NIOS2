/*
 *
 *  Created on: Nov 18, 2019
 *      Author: samiv
 *
 *      This file contains interrupt handlers and initialisation functions for the flash timer, responsible for turning on and off LEDs on
 *      an constant frequency.
 */

#include "port.h"

void flash_timer_init () {

	int context_ft;

	//Load default timer value
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE,5000000);

	//Debug read
	int periodl_reg = IORD_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE);
	int periodh_reg = IORD_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE);

	//Enable continuous mode and start bit
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_CONT_MSK);

	//Register isr
	alt_ic_isr_register(0x0,TIMER_1_IRQ, isr_flash_timer, &context_ft, 0x0);

	//Enable interrupt
	alt_ic_irq_enable(0,TIMER_1_IRQ);
}


void isr_flash_timer (void *context) {

  // clear the interrupt
  IOWR_ALTERA_AVALON_TIMER_STATUS (TIMER_1_BASE, 0);

  //Invert led_status to produce flash effect
  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,~led_status);
}

