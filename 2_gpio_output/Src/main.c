
/*Download STM32Cube MCU Package for STM32F4 series from st.com.
 Once you have downloaded the header file package , make sure to complete the below setting everytime you create a new project.
 1) While creating a new project , create an empty project.
 2) Right click the project folder and select properties , select paths and symbols.Select symbols and add STM32F411xE.
 3) Under paths and symbols select include and add these two paths
    A)D:\STM32\STM32Cube_FW_F4_V1.28.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include
    B) D:\STM32\STM32Cube_FW_F4_V1.28.0\Drivers\CMSIS\Include */
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
		GPIOA->ODR ^= LED_PIN; // access output data register (ODR) = toggle operator(^) led pin
		for(int i=0;i<100000;i++){}



}
}
