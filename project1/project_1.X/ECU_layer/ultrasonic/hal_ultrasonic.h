/* 
 * File:   hal_ultrasonic.h
 * Author: mokhtar
 *
 * Created on April 8, 2024, 3:31 PM
 */

#ifndef HAL_ULTRASONIC_H
#define	HAL_ULTRASONIC_H

#include"../../MKAL_layer/gpio/pro_gpio.h"
#include"../../MKAL_layer/timer1/hal_timer1.h"

typedef struct {
    pin_config_t trigger_pin; //output pin
    pin_config_t ECHO_pin;    //input pin
}ultra_pins_t;


void ultrasonic_initialize(ultra_pins_t *ultra_pins);
uint8 distance_ultrasonic(ultra_pins_t *ultra_pins);

#endif	/* HAL_ULTRASONIC_H */

