/*
 * exti.h
 *
 *  Created on: Jun 17, 2025
 *      Author: Aishita
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "stm32f4xx.h"
void pc13_exti_init(void);

/*Line 13 is bit 13 in the pending register. There's a register called pending.*/
#define LINE13 (1U<<13)


#endif /* EXTI_H_ */
