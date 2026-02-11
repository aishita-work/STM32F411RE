
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "systick.h"
#include "uart.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5
static void systick_callback(void);
int main (void){

	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	uart2_tx_init();
	systick_1hz_interrupt();

	while(1){
	}
}

static void systick_callback(void)
{
	printf("A second passed !! \n\r");
	GPIOA->ODR ^=LED_PIN;
}
/*Enable systick_1Hz_interrupt.The interrupt request handler for SysTick is called SysTick_Handler.
 So, this is the interrupt request handler for SysTick.Here we want to toggle the LED, and then print something.
 We will toggle the LED in a callback function.*/
void SysTick_Handler(void)  /*this is our SysTick handler, and this name here is also defined in the vector table. So, that's how I know exactly what to write here.*/
{
	//do something..

	systick_callback();
}

/*Run as STM32 project your LED should be toggling at a 1Hz rate. It is printing at 1Hz, and all of this is happening from our interrupt request handler. */







