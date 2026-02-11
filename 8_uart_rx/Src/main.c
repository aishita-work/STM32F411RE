/*changes made in uart.c to enable reciever , follow the comments in uart.c and changes in uart.h file*/
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"

//enabling LED
#define GPIOAEN     (1U<<0)
#define GPIOA_5     (1U<<5)
#define LED_PIN     GPIOA_5

//creating a new variable which will store the received value
char key  ; // key is going to be the value that we get when we read out UART2.

/*HOW TO PERFORM THE CODE*/
/*build main.c . right click on the main.c file in src and click on debug as and stm32 c/c++. click on switch.
 * after clicking on switch , a debug window will open in rightmost screen . click on live expression. add expression - key.
 * then open realterm and write d,g or anything on the terminal , you will receiver 49/'1' in key and led will blink.*/
int main(void){

	RCC->AHB1ENR |=GPIOAEN; //enable clock access to GPIOA

	GPIOA->MODER |= (1U<<10); //set PA5 as output pin
	GPIOA->MODER &=~ (1U<<11);



	uart2_rxtx_init(); // initialize uart2_rxtx_init function

	while(1){
		key = uart2_read();
		if(key =='1'){
			GPIOA->ODR |= LED_PIN;  //if key = 1 , turn on the led , else turn off
		}
		else{
			GPIOA->ODR &=~LED_PIN;
		}

	}

}


