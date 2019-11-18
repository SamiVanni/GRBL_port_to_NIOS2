/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include "port.h"

int main()
{
  printf("Hello from Nios II!\n");

  int delay = 5000000;
  int toggle1 = 1;
  int toggle2 = 1;
  int toggle3 = 1;
  int toggle4 = 1;

  int toggle7 = 1;
  int toggle8 = 1;

  uint8_t input = 0;

  serial_init();
  flash_timer_init();

  while(1){

	input = serial_read();

	led_status = IORD_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE);

	  if (input == '1') {
		if (toggle1 == 1) {
		  toggle1 = 0;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status | 0x01 );
		} else {
		  toggle1 = 1;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status & ~0x01 );
		}
	  }

	  if (input == '2') {
		if (toggle2 == 1) {
		  toggle2 = 0;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status | 0x02 );
		} else {
		  toggle2 = 1;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status & ~0x02 );
		}
	  }

	  if (input == '3') {
		if (toggle3 == 1) {
		  toggle3 = 0;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status | 0x04 );
		} else {
		  toggle3 = 1;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status & ~0x04 );
		}
	  }

	  if (input == '4') {
		if (toggle4 == 1) {
		  toggle4 = 0;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status | 0x08 );
		} else {
		  toggle4 = 1;
		  IOWR_ALTERA_AVALON_PIO_DATA(OUTPUT_PINS_BASE,led_status & ~0x08 );
		}
	  }

	  //decrease frequency
	  if (input == '5') {
		if (delay > 1000000) {
		  delay = delay - 1000000;
		  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE,delay);
		}
	  }

	  //increase frequency
	  if (input == '6') {
		if (delay < 10000000) {
		  delay = delay + 1000000;
		  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE,delay);
		}
	  }
	  //Flash mode
	  if (input == '7') {
		int timer_creg = IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE);

		if (toggle7 == 1) {
		  toggle7 = 0;

		  //Enable interrupt from the timer control register
		  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, timer_creg | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);

		} else {
		  toggle7 = 1;

		  //Disable interrupt from the timer control register
		  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, timer_creg & ~ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
		}
	  }

	  //Output mode
	  if (input == '8') {
		if (toggle8 == 1) {
		  toggle8 = 0;
		} else {
		  toggle8 = 1;
		}
	  }

	  if ((toggle8 == 1) & input != 0xFF) {
		  serial_write(input);
	  }

  }

  return 0;
}
