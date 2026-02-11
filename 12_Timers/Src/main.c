/*_______________________________________TIMERS THEORY___________________________________________________

 TIMER - Uses
 1)Counting Events
 2)Creating Delays
 3)Measuring time between event
 _____________________________________TIMER      VS       COUNTER _____________________________________________
                                       |                     |
                                       |                     |
                                       |                     |
                            Internal Clock Source      External Clock Source
                            Eg: PLL,XTAL,RC             Eg: Clock fed to CPU

 ________________________________________TIMER - STM32 Timers ________________________________________________________________________________________
 1)Can be used as time base generator.
 2)Can be used to measure the frequency of an external event - Input Capture Mode.
 3)Control an output waveform , or to indicate when a period of time has elapsed - Output Compare Mode.
 4)One plus mode(OPM) - allows the counter to be started in response to a stimulus and to generate a
                        pulse with a programmable length after a programmable delay.

 ________________________________________TIMER - Registers ____________________________________________________________________________________________________________________
 1)Timer Count Register(TIMx_CNT)-->       Shows the current value.Size could be 32-bit or 16-bit depending on timer module used.
 2)Timer Auto-Reload Register(TIMx-ARR)--> Timer raises a flag and the counter restarts automatically when counter value reaches
                                           the value in the auto-reload register.The counter is an up counter by default but can
                                           also be configured to be a down counter.
 3)Timer Prescaler Register(TIMx_PSC)-->   The prescaler slows down the counting speed of the timer by dividing the input clock of the timer.

_________________________________________TIMER - Clock Pre-scaling____________________________________________________________________________________________________________


                                                                    APB1 Prescaler (Divide by :1,2,4,8,16) ------> TIMx_PSC (Divide by : 1,2,4,8,65535) ------> to TIM_CNT
                                                                           ^
                                                                           |
                                                                           |
 SYSCLK ---->     AHB Prescaler       ----------->  HCLK ------------------
                        ^                             ^                    |
            (Divide by :1,2,4,8..512)         (Divide by :1,2,4,8..256)    |
                                                                           ▽
                                                                   APB2 Prescaler (Divide by :1,2,4,8,16) ------> TIMx_PSC (Divide by : 1,2,4,8,65535) ------> to TIM_CNT

 1)Timer prescaler (TIMx_PSC) determines how fast the timer counter (TIMx_CNT) increases/decreases.
 2)With each change in the counter(TIMx_PSC) value, the new value is compared to the value in the timer auto-reload register(TIM_ARR), when the values match,
   a flag is raised and an interrupt occurs.

_______________________________________TIMER - Some Terms_____________________________________________________________________________________________________________________
1)Update Event - When timeout occurs or how long it takes for flag to be raised.
2)Period- Value loaded into auto-reload register(TIM_ARR)
3)Up counter - Counts from zero to a set value.
4)Down counter - Counts from a set value down to zero.

________________________________________TIMER- Computing Update Event_______________________________________________________________

Update Event = Timer (clock) /(Prescaler+1)(Period+1)

Example : Timer clock  = APB1 clock    = 48 MHz
          Prescaler    = TIM_PSC value = 47999+1
          Period       = TIM_ARR value = 499+1

Update Event = 48 000 000/(4799+1)(499+1) = 2Hz = 1/2 sec = 0.5 sec

_________________________________TIMER-Registers(any timer apart from SysTick)________________________________________________________________________________________________
1)Prescaler(PSC) - prescaler value is put here .                                 Eg: TIM->PSC = 1600-1;  //S]et pescaler value to 1600
2)Auto-Reload Register (ARR) - Auto-reload value is put here .                   Eg: TIM2->ARR = 10000;  //Set prescaler value to 1600
3)Control Register 1 (CR1) - Enabling and disabling timer.                       Eg: TIM2->CR1;  //Enable timer2
4)Control Register (CR2)- For enabling/disabling the ADC.                        Eg: ADC1->CR2;  //Enable channel 1
5)Status Register (SR) - Checking , setting and clearing the flags of the timer. Eg: TIM2->SR&1;  //Check update intrrupt flag.  Eg: TIM2->SR & =~1; //Clear update interrupt flag

6)Capture/Compare Register(CCR1,CCR2,CCR3,CCR4) - One capture/compare register for each of the 4 channels. Eg: timestamp = TIM2->CCR1;  //read captured value
7)Capture Compare Mode Register 1 (CCMR1)       - Configuring capture/compare functionality for CH1 and CH2.
8)Capture Compare Mode Register 2 (CCMR2)       - Configuring capture/compare functionality for CH3 and CH4. Eg: TIM2->CCMR1 = 0X41;  //set CH1 to capture at every edge
9)Capture/Compare Enable Register (CCER)        - Used to enable any of the timer channels either as input capture or output capture. Eg: TIM2->CCER = 1;  //enable channel 1

In this project we develop a driver for our hardware timers, so these timers are different from systick. These are timers added by the silicon manufacturer, the timers added by STMicroelectronics.
 These are known as General Purpose Timers, but often the general purpose timer is a generic name used to describe these timers. Within these timers as these are the groups known as the general
purpose group, the advanced timers and then the basic timers.Overall we call all of them as general-purpose timers to distinguish them from the SysTick timer .
 */


#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "timer.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5

int main (void){

	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	uart2_tx_init();

	/*Just like in SysTick, we waited for SysTick count flag to be set in in order for us to know the time has occured.In the TIM2 ,
	 we have to wait for a particular flag to be set to indicate the time out has occured.This is known as the UIF or update interrupt
	 flag his is located in the timer status register.//Search TIMx_SR. Bit zero here is UIF. When this is set it shows that our time
	 out that we set, has just occurred once again or has just passed. So we have created a symbolic name for this in timer.h .*/
	tim2_1hz_init();

	while(1){

		while(!(TIM2->SR  & SR_UIF)){}   //wait for UIF flag to be set. when this is false, we want to get stuck here that's how we perform the wait

			/*Clear UIF once it has occurred*/
			TIM2->SR  &=~SR_UIF;

		printf("A second passed .. \n\r");
		GPIOA->ODR ^=LED_PIN;

	}
}
/*Run code. Check realTerm application. Set baud rate and select open,it will show print statement : a second passed.*/
