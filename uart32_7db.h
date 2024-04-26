#ifndef USART_H
#define USART_H
/************************************************************************
Title:    Interrupt USART library with receive/transmit circular buffers
Author:   
File:     $Id: uart.h,v 1.0 2006/05/23 11:25:41 $
Software: AVR-GCC 3.3
Hardware: any AVR with built-in USART, tested on ATmega32 at 8 Mhz
Usage:    
************************************************************************/

/** 
 *  @defgroup pfleury_uart USART Library
 *  @code #include <uart.h> @endcode
 * 
 *  @brief Interrupt USART library using the built-in USART with transmit and receive circular buffers. 
 *
 *  This library can be used to transmit and receive data through the built in USART. 
 *
 *  An interrupt is generated when the USART has finished transmitting or
 *  receiving a byte. The interrupt handling routines use circular buffers
 *  for buffering received and transmitted data.
 *
 *  The USART_RX_BUFFER_SIZE and USART_TX_BUFFER_SIZE constants define
 *  the size of the circular buffers in bytes. Note that these constants must be a power of 2.
 *  You may need to adapt this constants to your target and your application by adding 
 *  CDEFS += -DUSART_RX_BUFFER_SIZE=nn -DUSART_RX_BUFFER_SIZE=nn to your Makefile.
 *
 *  @note Based on Atmel Application Note AVR306
 *  @author Peter Fleury pfleury@gmx.ch  http://jump.to/fleury
 */
 



#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif


/*
** constants and macros
*/

/** @brief  USART Baudrate Expression
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define USART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** @brief  USART Baudrate Expression for ATmega double speed mode
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz           
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define USART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)


/** Size of the circular receive buffer, must be power of 2 */
#ifndef USART_RX_BUFFER_SIZE
#define USART_RX_BUFFER_SIZE 32
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef USART_TX_BUFFER_SIZE
#define USART_TX_BUFFER_SIZE 32
#endif

/* test if the size of the circular buffers fits into SRAM */
#if ( (USART_RX_BUFFER_SIZE+USART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
#error "size of USART_RX_BUFFER_SIZE + USART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

/* 
** high byte error return code of uart_getc()
*/
#define USART_FRAME_ERROR      0x0800              /* Framing Error by USART       */
#define USART_OVERRUN_ERROR    0x0400              /* Overrun condition by USART   */
#define USART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define USART_NO_DATA          0x0100              /* no receive data available   */


/*
** function prototypes
*/

/**
   @brief   Initialize USART and set baudrate 
   @param   baudrate Specify baudrate using macro USART_BAUD_SELECT()
   @return  none
*/
extern void uart_init(unsigned int baudrate);


/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the 
 * higher byte the last receive error.
 * USART_NO_DATA is returned when no data is available.
 *
 *  @param   void
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from USART
 *           - \b USART_NO_DATA           
 *             <br>no receive data available
 *           - \b USART_BUFFER_OVERFLOW   
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough, 
 *             one or more received character have been dropped 
 *           - \b USART_OVERRUN_ERROR     
 *             <br>Overrun condition by USART.
 *             A character already present in the USART UDR register was 
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b USART_FRAME_ERROR       
 *             <br>Framing Error by USART
 */
extern unsigned int uart_getc(void);


/**
 *  @brief   Put byte to ringbuffer for transmitting via USART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart_putc(unsigned char data);


/**
 *  @brief   Put string to ringbuffer for transmitting via USART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the USART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart_puts(const char *s );


/**
 * @brief    Put string from program memory to ringbuffer for transmitting via USART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the USART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s );

/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))





#endif // USART_H 

