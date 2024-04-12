/* 
 * File:   hal_ultrasonic.h
 * Author: mokhtar
 *
 * Created on April 8, 2024, 3:31 PM
 */

#include"hal_ultrasonic.h"


void  send_start(ultra_pins_t *ultra_pins);
uint8 measure_distance_(ultra_pins_t *ultra_pins);


void ultrasonic_initialize(ultra_pins_t *ultra_pins){
    ultra_pins->ECHO_pin.logic=GPIO_OUTPUT;
    ultra_pins->ECHO_pin.logic=GPIO_INPUT;
    GPIO_pIN_INTIALIZE(&(ultra_pins->ECHO_pin));
    GPIO_pIN_INTIALIZE(&(ultra_pins->trigger_pin));
    
}

uint8 distance_ultrasonic(ultra_pins_t *ultra_pins){
    uint8 l_variable=0;
    send_start(ultra_pins);
    l_variable = measure_distance_(ultra_pins);
    return (l_variable );
}



void  send_start(ultra_pins_t *ultra_pins){
    GPIO_pIN_WRITE_LOGIC(&(ultra_pins->trigger_pin),GPIO_HIGH);  // Set trigger pin high
    __delay_us(10);                                           // Wait for 10 microseconds
    GPIO_pIN_WRITE_LOGIC(&(ultra_pins->trigger_pin),GPIO_LOW);  // Set trigger pin low
}


 uint8 measure_distance_(ultra_pins_t *ultra_pins) {
    uint16 pulse_width = 0;
    uint8 variable = 0;
    GPIO_pIN_READ_LOGIC(&(ultra_pins->ECHO_pin),&variable);
    while (!variable){
        GPIO_pIN_READ_LOGIC(&(ultra_pins->ECHO_pin),&variable);
    } // Wait for ECHO pin to go high
    TMR1 = 0;
    TIMER1_ENABLE();  // Reset Timer1
    while (variable) {
        GPIO_pIN_READ_LOGIC(&(ultra_pins->ECHO_pin),&variable);
        if (TMR1 > 60000)   // Break the loop if pulse is too long (timeout)
            break;
    }
    pulse_width = TMR1;     // Read Timer1 value
    
    // Convert pulse width to distance (example, adjust according to sensor specs)
    return (uint16)(pulse_width / 58); // Divide by 58 for centimeters (example)
}