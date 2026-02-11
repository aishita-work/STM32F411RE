#include "stm32f4xx.h"

#define GPIOAEN    (1U<<0)  //shift 1 to bit 0
#define PIN5       (1U<<5)  //shift 1 to position 5
#define LED_PIN    PIN5     //led pin is pin 5

int main(void)  //takes no argument here , using void
{
	RCC->AHB1ENR |=GPIOAEN;  // enable clock access for GPIOA
	GPIOA->MODER |=(1U<<10);  //going to mode register to set PA5 as an output pin(setting bit 10 to 1)
	GPIOA->MODER &=~(1U<<11);  //set bit 11 to 0

	while(1){
		GPIOA->BSRR = LED_PIN;    //here LED_PIN already set to 1 at position 5 which is BS5
		for(int i=0;i<100000;i++){}

		GPIOA-> BSRR = (1U<<21);  //set 1 at position 21 (BR5 at position 21) means turn 0ff the led
		for(int i=0;i<100000;i++){}


}
}
