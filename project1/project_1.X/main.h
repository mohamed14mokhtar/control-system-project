/* 
 * File:   main.h
 * Author: mokhtar
 *
 * Created on April 1, 2024, 2:38 PM
 */

#ifndef MAIN_H
#define	MAIN_H

/*section : includes */


#include"ECU_layer/ecu_layer_init.h"
#include"MKAL_layer/Interrupt/mcal_external_interrupt.h"
#include"MKAL_layer/Interrupt/mcal_interrupt_config.h"
#include"MKAL_layer/Interrupt/mcal_interrupt_manager.h"
#include"MKAL_layer/EEPROM/hal_eeprom.h"
#include"MKAL_layer/ADC/hal_adc.h"
#include"MKAL_layer/timer/hal_timer0.h"
#include"MKAL_layer/timer1/hal_timer1.h"
#include"MKAL_layer/timer2/hal_timer2.h"
#include"MKAL_layer/timer3/hal_timer3.h"
#include"MKAL_layer/CCP1/hal_ccp1.h"
#include"MKAL_layer/CCP2/hal_ccp2.h"
#include"MKAL_layer/USART/hal_usart.h"
#include"ECU_layer/TC74/hal_tc74.h"
#include"ECU_layer/ultrasonic/hal_ultrasonic.h"
/*section : macro declarations  */
#define _XTAL_FREQ 4000000UL

/*section : macro declaration functions  */


/*section : data type declarations  */


/*section : functions declarations  */
void intitialized_functions (void);


#endif	/* MAIN_H */

