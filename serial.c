/*
  serial.c - Low level functions for sending and recieving bytes via the serial port
  Part of Grbl

  Copyright (c) 2011-2015 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/
// TODO: SAMI PORT ENABLOINTIA
// #include "grbl.h"
#include "port.h"

uint8_t serial_rx_buffer[RX_BUFFER_SIZE];
uint8_t serial_rx_buffer_head = 0;
volatile uint8_t serial_rx_buffer_tail = 0;

uint8_t serial_tx_buffer[TX_BUFFER_SIZE];
uint8_t serial_tx_buffer_head = 0;
volatile uint8_t serial_tx_buffer_tail = 0;


#ifdef ENABLE_XONXOFF
  volatile uint8_t flow_ctrl = XON_SENT; // Flow control state variable
#endif
  

// Returns the number of bytes used in the RX serial buffer.
uint8_t serial_get_rx_buffer_count()
{
  uint8_t rtail = serial_rx_buffer_tail; // Copy to limit multiple calls to volatile
  if (serial_rx_buffer_head >= rtail) { return(serial_rx_buffer_head-rtail); }
  return (RX_BUFFER_SIZE - (rtail-serial_rx_buffer_head));
}


// Returns the number of bytes used in the TX serial buffer.
// NOTE: Not used except for debugging and ensuring no TX bottlenecks.
uint8_t serial_get_tx_buffer_count()
{
  uint8_t ttail = serial_tx_buffer_tail; // Copy to limit multiple calls to volatile
  if (serial_tx_buffer_head >= ttail) { return(serial_tx_buffer_head-ttail); }
  return (TX_BUFFER_SIZE - (ttail-serial_tx_buffer_head));
}


void serial_init()
{

	int context_uart;

	//Hardware has fixed baud rate at 115200

	//Register isr
	alt_ic_isr_register(0x0,UART_RS232_IRQ, isr_uart_rs232, &context_uart, 0x0);

	//Enable interrupt
	alt_ic_irq_enable(0,UART_RS232_IRQ);
	
	//Enable rx interrupt
	IOWR_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE,ALTERA_AVALON_UART_CONTROL_RRDY_MSK);

}


// Writes one byte to the TX serial buffer. Called by main program.
// TODO: Check if we can speed this up for writing strings, rather than single bytes.
void serial_write(uint8_t data) {
  // Calculate next head
  uint8_t next_head = serial_tx_buffer_head + 1;
  if (next_head == TX_BUFFER_SIZE) { next_head = 0; }

  // Store data and advance head
  serial_tx_buffer[serial_tx_buffer_head] = data;
  serial_tx_buffer_head = next_head;
  
  // Enable Data Register Empty Interrupt to make sure tx-streaming is running
  int control_reg = IORD_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE);
  IOWR_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE, control_reg | ALTERA_AVALON_UART_STATUS_TRDY_MSK);

 }

void isr_uart_rs232 (void *context)
{
  int sr;

  int cr; //Debug variables

  //Reading STATUS register
  sr = IORD_ALTERA_AVALON_UART_STATUS(UART_RS232_BASE);

  //Reading CONTROL register
  cr = IORD_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE);

  // Data Register Empty Interrupt handler
  if (((sr & ALTERA_AVALON_UART_STATUS_TRDY_MSK) & (cr & ALTERA_AVALON_UART_STATUS_TRDY_MSK)) == ALTERA_AVALON_UART_STATUS_TRDY_MSK) {

    uint8_t tail = serial_tx_buffer_tail; // Temporary serial_tx_buffer_tail (to optimize for volatile)

    // Send a byte from the buffer
    IOWR_ALTERA_AVALON_UART_TXDATA(UART_RS232_BASE,serial_tx_buffer[tail]);

    //Reading STATUS register, DEBUG
    sr = IORD_ALTERA_AVALON_UART_STATUS(UART_RS232_BASE);
    int sent_data = serial_tx_buffer[tail];

    // Update tail position
    tail++;
    if (tail == TX_BUFFER_SIZE) { tail = 0; }

    serial_tx_buffer_tail = tail;

    // Turn off Data Register Empty Interrupt to stop tx-streaming if this concludes the transfer
    if (tail == serial_tx_buffer_head) {
    	int control_reg = IORD_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE);
    	IOWR_ALTERA_AVALON_UART_CONTROL(UART_RS232_BASE, control_reg & ~ALTERA_AVALON_UART_STATUS_TRDY_MSK);
    }
  }

  //RX interrupt handler
  if (((sr & ALTERA_AVALON_UART_STATUS_RRDY_MSK) & (cr & ALTERA_AVALON_UART_CONTROL_RRDY_MSK)) == ALTERA_AVALON_UART_STATUS_RRDY_MSK) {

	  uint8_t data = IORD_ALTERA_AVALON_UART_RXDATA(UART_RS232_BASE);
	  uint8_t next_head;

// TODO: SAMI PORT ENABLOINTIA
//	  switch (data) {
//	    case CMD_STATUS_REPORT: bit_true_atomic(sys_rt_exec_state, EXEC_STATUS_REPORT); break; // Set as true
//	    case CMD_CYCLE_START:   bit_true_atomic(sys_rt_exec_state, EXEC_CYCLE_START); break; // Set as true
//	    case CMD_FEED_HOLD:     bit_true_atomic(sys_rt_exec_state, EXEC_FEED_HOLD); break; // Set as true
//	    case CMD_SAFETY_DOOR:   bit_true_atomic(sys_rt_exec_state, EXEC_SAFETY_DOOR); break; // Set as true
//	    case CMD_RESET:         mc_reset(); break; // Call motion control reset routine.
//	    default: // Write character to buffer
	      next_head = serial_rx_buffer_head + 1;
	      if (next_head == RX_BUFFER_SIZE) { next_head = 0; }

	      // Write data to buffer unless it is full.
	      if (next_head != serial_rx_buffer_tail) {
	        serial_rx_buffer[serial_rx_buffer_head] = data;
	        serial_rx_buffer_head = next_head;

// TODO: SAMI PORT ENABLOINTIA
//	      }
	      //TODO: else alarm on overflow?
	  }

  }
}


// Fetches the first byte in the serial read buffer. Called by main program.
uint8_t serial_read()
{
  uint8_t tail = serial_rx_buffer_tail; // Temporary serial_rx_buffer_tail (to optimize for volatile)
  if (serial_rx_buffer_head == tail) {
    return SERIAL_NO_DATA;
  } else {
    uint8_t data = serial_rx_buffer[tail];
    
    tail++;
    if (tail == RX_BUFFER_SIZE) { tail = 0; }
    serial_rx_buffer_tail = tail;

    #ifdef ENABLE_XONXOFF
      if ((serial_get_rx_buffer_count() < RX_BUFFER_LOW) && flow_ctrl == XOFF_SENT) { 
        flow_ctrl = SEND_XON;
        UCSR0B |=  (1 << UDRIE0); // Force TX
      }
    #endif
    
    return data;
  }
}



void serial_reset_read_buffer() 
{
  serial_rx_buffer_tail = serial_rx_buffer_head;

  #ifdef ENABLE_XONXOFF
    flow_ctrl = XON_SENT;
  #endif
}
