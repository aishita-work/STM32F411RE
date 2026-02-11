/*_______________________SYSTEM TICK TIMER(SYSTICK) THEORY_________________________________________________________

 ___________________________SYSTICK-INTRODUCTION___________________________________________________________________
 1) Found in all ARM Cortex-Microcontrollers regardless of silicon manufacturer.
 2)Used for taking actions periodically.Often used as time-base for real-time operating systems.
 3)The Systick is a 24-bit down counter driven by the processor clock.

 ______________________________SYSTICK-Counting____________________________________________________________________
 1)Counts from initial value down to zero.
 2)24-bit imply maximum initial value of: 2^24= 0xFFFFFF = 16,777,216
 3)Initial value can be set to a value between 0x000000 to 0xFFFFFF

 ______________________________SYSTICK-Registers___________________________________________________________________
 1)Systick Current Value Register(STCVR) - This register contains the current count value.
 2)Systick Control & Status Register(STCSR)-This register allows us to configure the systick
   clock source, enable/disable interrupts and enable/disable the systick counter.
 3)Systick Reload Value Register(STRVR)-This is where the initial count value is placed

  ______________________SYSTICK-Count value computation____________________________________________________________
  Compute the delay achieved by loading 10 in the Systick Reload Value Register(STRVR) given system clock =16MHz
  Written as: Systick->LOAD = 9
             *written in the CMSIS standard
             *we write 9 although we want 10 because the counter starts counting from 0
  Solution: System clock =16MHz = 16 000 000 cycles/second
  If 1 second executes 16 000 000 cycles, how many seconds executes 1 cycle?
  => 1/(16000000) = 62.5ns = 62.5x10^(-9) s
  Then  10 cycles => 10x62.5x10^(-9) s = 625 x10^(-9)s = 625ns
  By writing Systick->LOAD=9, we will get 625ns delay or timeout.

  System Clock (SYSCLK) is chosen as clock source.
 If:  Systick->LOAD =N
  Delay achieved = N x 1/(SYSCLK) = N/(SYSCLK)
  For STM32F4 Nucleo board the default system clock is 16MHz.

  Eg: Compute N value for achieving a 1ms delay given SYSCLK as 16MHz
  Solution:    1ms = 0.001s
           Delay = N/SYSCLK
           0.001 = N/(16 000 000)
              N  = 0.001 x (16 000 000)
              N  = 16000
_________________________________________THEORY COMPLETE__________________________________*/

/*Check systick.h and systick.c file for main code*/


#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "systick.h"
#include "uart.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5

int main (void){

	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	uart2_tx_init();

	while(1){

		printf("A second passed .. \n\r");
		GPIOA->ODR ^=LED_PIN;
		systickDelayMs(1000);  //delay for 1000ms = 1 sec
	}
}
/*Run code. Check realTerm application. Set baud rate and select open,it will show print : a second passed.*/
