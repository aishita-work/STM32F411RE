
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include <uart.h>

//enabling LED
#define GPIOAEN     (1U<<0)
#define GPIOA_5     (1U<<5)
#define LED_PIN     GPIOA_5

//creating a new variable which will store the received value
char key  ; // key is going to be the value that we get when we read out UART2.

int main(void)
{
	RCC->AHB1ENR |=GPIOAEN; //enable clock access to GPIOA

	GPIOA->MODER |= (1U<<10); //set PA5 as output pin
	GPIOA->MODER &=~ (1U<<11);
    uart2_rxtx_interrupt_init();
}
// In our callback function, we want to read the USAT data register.
static void uart_callback(void)
{
	key = USART2->DR;
	if(key =='1')
	{
		GPIOA->ODR |= LED_PIN;
	}
	else{
		GPIOA->ODR &= ~LED_PIN;
	}
}

/*Because we're dealing with interrupt, we need to implement the uart interrupt request handler.
 If we go to the NVIC, we'd find that the interrupt for usart2 is called USART2_IRQ handler.
 So, this is a void void function.*/
void USART2_IRQHandler(void)
{
	/*Check if RXNE is set*/
	 /*Earlier over here(in uart2_read() function), we were waiting for the status register RXNE to be set.
	  This time, we don't wait. Remember ,whenever we add an interrupt request handler, there is no waiting.
	  We don't have to put blocking code in there.Earlier for uart_2read() we used while. This time here,
	  we just check if it's set. That is different from waiting.To check , we use if .To wait,we use while.*/
	if(USART2->SR & SR_RXNE)  //if we read the status register RXNE, if this is set, then our interrupt has occurred, and we can read the data.
	{
		//Do Something..
		uart_callback();
	}
}

/*Go to project folder, right click and select debug as. Click to switch tab for debug.
  click live expression tab and enter "key". Go to realTerm and select correct baudrate and port.
  then click on the black screen and enter any character or number. The LED will glow only when we enter 1. */

