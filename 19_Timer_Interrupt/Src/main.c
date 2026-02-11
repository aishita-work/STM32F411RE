

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "timer.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5


static void tim2_callback(void);
int main (void){

	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	uart2_tx_init();
	tim2_1hz_interrupt_init();

	/*Just like in SysTick, we waited for SysTick count flag to be set in in order for us to know the time has occured.In the TIM2 ,
	 we have to wait for a particular flag to be set to indicate the time out has occured.This is known as the UIF or update interrupt
	 flag his is located in the timer status register.//Search TIMx_SR. Bit zero here is UIF. When this is set it shows that our time
	 out that we set, has just occurred once again or has just passed. So we have created a symbolic name for this in timer.h .*/
	tim2_1hz_init();

	while(1){
	}
}
static void tim2_callback(void)      //because we are dealing with interrupt, we need no wait for flags unlike we did in 11_Timers
{
	printf("A second passed .. \n\r");
	GPIOA->ODR ^=LED_PIN;
}

/*Inside main.c we're going to implement our Interrupt Request Handler, so the Interrupt Request Handler for Timer2 is TIM2_IRQHandler.*/
void TIM2_IRQHandler(void){
/*the first thing we want to do is when the interrupt occurs, we want to clear the UIF, or clear the update interrupt flag. And this time
we'll clear it in the status register. We enable it in the DMA/interrupt enable register, but we'll clear it in the status register.
We created a symbolic name for this, SR_UIF in timer.h.*/
	TIM2->SR &=~SR_UIF;
	//Do something.....
	tim2_callback();
}

/*built project successfully.Run As, STM32 C/C++ application.Your LED should be blinking and we can confirm here together by seeing real-time.
 Our callback function says print a second pass. So at 1 Hz, it's printing this.*/




