/*
 * systick.c
 *
 *  Created on: May 17, 2025
 *      Author: Aishita
 */
#include "stm32f4xx.h"

#define SYSTICK_LOAD_VAL      16000  //Default clock is 16MHz,meaning default clock is 16 million cycles per second.Therefore, 16000 cycles happen in a single millisecond

/*Systick timer is core peripheral of the cortex M architecture, information regarding the system doesn't exist in any of the ST microelectronics documents.If we want complete information,
 the bare metal information of systick, we need to take a look at a generic user guide of the cortex M which is provided by ARM company.*/

/*To enable systick timer, go to generic user guide.Go to systick control and status register. Bit zero here , when we set it to 1, we enable it*/
#define CTRL_ENABLE  (1U<<0)

/*To decide a internal clock source, bit two decides the clock source. when we set bit two to 1, the processor clock source meaning system clock enables.If we had an external clock we wanted to use, we set bit two to 0.*/
#define CTRL_CLKSRC  (1U<<2)

/*Count Flag is bit sixteen.It returns 1 if timer counted to 0 since last time this was read. So we would be checking count flag whether there's a timeout.*/
#define CTRL_COUNTFLAG  (1U<<16)

/*The SysTick value is going to be 16 million, because our default clock is 16 MHz*/
#define ONE_SEC_LOAD  16000000

/*Go to our CortexM4 generic user guide. Search SysTick here. In SysTick Control and Status Register, bit number one here is called Tick Interrupt.It's function is to Enable SysTick Exception.*/
#define CTRL_TICKINT  (1U<<1)
void systickDelayMs(int delay){  //function takes number of milliseconds we want delay for.

	/*Configure systick - reload with number of clocks per millisecond*/
	SysTick->LOAD  = SYSTICK_LOAD_VAL;

	/*Clear systick current value register*/
	SysTick->VAL =0;

	/*Enable systick and select internal clock source*/
	SysTick->CTRL =  CTRL_ENABLE | CTRL_CLKSRC;

	for(int i=0; i<delay; i++){
		/*Wait until the COUNTFLAG is set*/
		while(( SysTick->CTRL & CTRL_COUNTFLAG )==0){}
	}
	SysTick->CTRL = 0; //write zero to SysTick control register, which would disable it.

}

/*Created here a new function called SysTick_1hz_interrupt.The name implies, this SysTick interrupt is going to occur every second, or once a second. So we're going to start off by loading our SysTick value.
 Over here the SysTick value is going to be 16 million, because our default clock, which is the internal clock that we are using as the source for SysTick, is 16 MHz, meaning 16 million cycles per second.
 We have created a symbolic name,ONE_SEC_LOAD. If you have a different microcontroller, this value will be different. It will be based on the default frequency of your system.
 Once this is done, we're going to clear the SysTick Current Value register.Now we're going to enable SysTick Interrupt and select Clock Source, like we did in systickDelayMs function but this time we need to enable Interrupt as well.
 So, let's go and find the bit for enabling Interrupt. Let's go to our CortexM4 generic user guide. I'll search SysTick here.In SysTick Control and Status Register, bit number one here is called Tick Interrupt.It's funtion is to Enable SysTick Exception.
 We're going to create this bit, number one,CTRL_TICKINT. We don't need to enable SysTick in the NVIC. Expose this function to SysTick.h  */
void systick_1hz_interrupt(void)
{
	/*SysTick->LOAD , reload with number of clocks per second*/
	SysTick->LOAD = ONE_SEC_LOAD -1;  /*because we count from zero, we say minus one*/
	SysTick->VAL =0; /*Clear systick current value register*/
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC; /*Enable systick and select internal clk src*/
	SysTick->CTRL |= CTRL_TICKINT; /*Enable systick interrupt*/

}






