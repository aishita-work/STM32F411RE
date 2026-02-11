/*
 * timer.h
 *
 *  Created on: May 22, 2025
 *      Author: Aishita
 */

#ifndef TIMER_H_
#define TIMER_H_
#define SR_UIF  (1U<<0)  //Search TIMx_SR. Bit zero here is UIF. When this is set it shows that our time out that we set, has just occurred once again or has just passed. So we have created a symbolic name for this in timer.h .

void tim2_1hz_init(void);

void tim2_pa5_output_compare(void);

#endif /* TIMER_H_ */
