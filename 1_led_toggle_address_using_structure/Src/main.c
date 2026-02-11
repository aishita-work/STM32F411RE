//where is the led connected?
//what is the port? A
//what is the pin? 5
#include <stdint.h>
#define PERIPH_BASE         (0x40000000UL)
#define AHB1PERIPH_OFFSET   (0x00020000UL)
#define AHB1PERIPH_BASE     (PERIPH_BASE + AHB1PERIPH_OFFSET) //0x4000 0000 + 0x002 0000 = 0x4002 0000
#define GPIOA_OFFSET        (0x0000UL)  //0x 0000 0000

#define GPIOA_BASE          (AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET          (0x3800UL)
#define RCC_BASE            (AHB1PERIPH_BASE + RCC_OFFSET)


#define AHB1EN_R_OFFSET     (0x30UL)
#define RCC_AHB1EN_R        (*(volatile unsigned int *)(RCC_BASE + AHB1EN_R_OFFSET))

#define MODE_R_OFFSET       (0x00UL)
#define GPIOA_MODE_R        (*(volatile unsigned int *)(GPIOA_BASE + MODE_R_OFFSET))

#define OD_R_OFFSET         (0x14UL)
#define GPIOA_OD_R          (*(volatile unsigned int *)(GPIOA_BASE + OD_R_OFFSET))

#define GPIOAEN             (1U<<0)  //0b 0000 0000 0000 0000 0000 0000 0000 0001 means shift 1 to position 0
#define PIN5                (1U<<5)
#define LED_PIN              PIN5

#define  __IO   volatile

#define RCC      ((MYRCC_TypeDef*) RCC_BASE)
#define GPIOA    ((MYGPIO_TypeDef*) GPIOA_BASE)
/*
typedef struct{

	__IO uint32_t MODER;     //GPIO port  mode register ,                     Address offset : 0x00
	__IO uint32_t OTYPER;    //GPIO port output type register ,               Address offset : 0x04
	__IO uint32_t OSPEEDR;   //GPIO port output speed register ,              Address offset : 0x08
	__IO uint32_t PUPDR;     //GPIO port pull-up/pull-down register ,         Address offset : 0x0C
	__IO uint32_t IDR;      //GPIO port input data register ,                 Address offset : 0x10
	__IO uint32_t ODR;      //GPIO port out-put data register ,               Address offset : 0x04
	__IO uint32_t BSRR;     //GPIO port bit set /reset register ,             Address offset : 0x08
	__IO uint32_t LCKR;     //GPIO port configuration lock register ,         Address offset : 0x0C
	__IO uint32_t AFR[2];   //GPIO port Alternative fuction register ,        Address offset : 0x20-0x24

}GPIO_TypeDef;
*/
typedef struct{
	volatile uint32_t DUMMY[12];
	volatile uint32_t AHB1ENR;
} MYRCC_TypeDef;

typedef struct{
	volatile uint32_t MODER;     //GPIO port  mode register ,                     Address offset : 0x00
	volatile uint32_t DUMMY[4]; // creates dummy register to remove the register we don't want in between
	volatile uint32_t ODR;      ////GPIO port out-put data register ,               Address offset : 0x04

} MYGPIO_TypeDef;


int main (void)
{
	//1. enable clock access to GPIOA
	//RCC_AHB1EN_R |= GPIOAEN;
	RCC->AHB1ENR |=GPIOAEN;

	//2. set PA5 as output pin
	//GPIOA_MODE_R |= (1U<<10);  // set bit 10 to 1
	//GPIOA_MODE_R &=~(1U<<11); // SET BIT 11 TO 0
/*To configure a single pin ,we need to toggle two bits , we need to configure two bits to configure the mode
 * of a single pin. Because we are dealing with PA5 , we are interested in MODER5 and MODER5 occupies
 * bit 10 and 11.what do we write at bits 10 and 11 to get MODER5 to be , PA5 to set as output?
 * we need to look at the truth table it tells us when we said two bit bits, 10 and 11 to zero zero ,then
 * PA5 will become an input pin, when we set bits 10 to 1 and bit 11 to zero , then PA5	 will become an output pin.
 * when we st bit 10 to 0 and bit 11 to 1, PA5 becomes an alternate function pin , it means it could be the Rx
 * or Tx line of UART Peripheral. when we set both pin to 0so we configure the pin as analog pin. here we
 * set this pin as an output pin(so we set bit 10 to 1 and bit 11 to 0).*/
	GPIOA->MODER |=(1U<<10);
	GPIOA->MODER &=~(1U<<11);

	while(1){
		//experiment 1 - PA5 high
		//GPIOA_OD_R |= LED_PIN;  //set PA5 high

		//experiment 2 - toggle PA5
		//GPIOA_OD_R ^= LED_PIN;  //set PA5 high and ^ is toggle operator
		GPIOA->ODR ^= LED_PIN;
		for (int i =0; i < 100000;i++){}

	}

}






