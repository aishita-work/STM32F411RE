#include "stm32f4xx.h"
// configuring push button(blue push button) on the nucleo board, we need to configure it to work as input.
#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
/* push button is connected to I/O PC13(pin 2) of STM32.before we configure a port,
 * we have to enable a port or a peripheral , we have to enable clock access to it.
 * now we introduce PC13 to our system.we have to find how we have to enable it's clock.
 * we have to know which bus it is connected to in order to enable it to clock.
 * for the bus info we go to block diagram (data sheet document). We see port C here has the arrow
 * touch in it and then touching AHB bus. Then we go through the AHB1 enable register then to enable access
 * to PORTC.Go to reference manual to see which bit in AHB1 enable registration is required.We see bit 2as
 * GPIOC. so we need to set bit 2 inside AHB1ENR to enable clock access to PORTC.
 * Also we are dealing with pin 13 , we need to create a symbolic name for pin 13*/
#define GPIOCEN    (1U<<2) //shift 1 to position 2
#define PIN13      (1U<<13)
#define BTN_PIN     PIN13

#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5

int main(void)  //takes no argument here , using void
{

	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA and GPIOC
	RCC->AHB1ENR |=GPIOCEN;
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	/*Set PC13 as input pin(whenever we want to set the mode , we need to go to MODE register).
	 * we are dealing with PIN13 . so we need to set MODER13. MODER13 occupies bit 26 and bit 27.
	 * For input we need to set both of the bits to zero. */
	GPIOC->MODER &=~(1U<<26); //set bit 26 to zero
	GPIOC->MODER &=~(1U<<27); //set bit 27 to zero

	while(1){

		/* check if BTN is pressed(our button is active LOW button means its initial state is high.
		 * It is pressed by default. when we press it , it becomes low.)*/
		if(GPIOC->IDR & BTN_PIN){ //IDR is input data register . if IDR and pin 13 are high
		GPIOA->BSRR = LED_PIN;    //turn ON LED

		}
		else{
		GPIOA-> BSRR = (1U<<21);  //turn OFF LED
		}
}
}
