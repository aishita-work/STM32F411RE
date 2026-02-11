/*creating uart.h and uart.c file in src folder and using header file in main.c*/
#include "stdio.h"
#include "stdint.h"
#include "stm32f4xx.h"
#include "uart.h"




int main(void){
	uart2_tx_init();

	while(1){
		printf("Hello from STM32....\n\r"); //go to realterm app.In port option correct the baudrate and click open and terminal should show hello from stm32
	}

}



