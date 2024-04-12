
#include"main.h"
#include "ECU_layer/button/ecu_button.h"
#include "MKAL_layer/I2C/hal_i2c.h"

#define slave_address 0x60

/********************************************* KEYPAD ***********************************************************/
 matrix_keypad_t matrix_keypad ={
.keymap_rows_pins[0].port=PORTD_INDEX,.keymap_rows_pins[0].direction=GPIO_OUTPUT,.keymap_rows_pins[0].logic=GPIO_LOW,.keymap_rows_pins[0].pin=pin0,
.keymap_rows_pins[1].port=PORTD_INDEX,.keymap_rows_pins[1].direction=GPIO_OUTPUT,.keymap_rows_pins[1].logic=GPIO_LOW,.keymap_rows_pins[1].pin=pin1,
.keymap_rows_pins[2].port=PORTD_INDEX,.keymap_rows_pins[2].direction=GPIO_OUTPUT,.keymap_rows_pins[2].logic=GPIO_LOW,.keymap_rows_pins[2].pin=pin2,
.keymap_rows_pins[3].port=PORTD_INDEX,.keymap_rows_pins[3].direction=GPIO_OUTPUT,.keymap_rows_pins[3].logic=GPIO_LOW,.keymap_rows_pins[3].pin=pin3,
.keymap_colums_pins[0].port=PORTD_INDEX,.keymap_colums_pins[0].direction=GPIO_INPUT,.keymap_colums_pins[0].logic=GPIO_LOW,.keymap_colums_pins[0].pin=pin4,
.keymap_colums_pins[1].port=PORTD_INDEX,.keymap_colums_pins[1].direction=GPIO_INPUT,.keymap_colums_pins[1].logic=GPIO_LOW,.keymap_colums_pins[1].pin=pin5,
.keymap_colums_pins[2].port=PORTD_INDEX,.keymap_colums_pins[2].direction=GPIO_INPUT,.keymap_colums_pins[2].logic=GPIO_LOW,.keymap_colums_pins[2].pin=pin6,
.keymap_colums_pins[3].port=PORTD_INDEX,.keymap_colums_pins[3].direction=GPIO_INPUT,.keymap_colums_pins[3].logic=GPIO_LOW,.keymap_colums_pins[3].pin=pin7
};
/********************************************* I2C ***********************************************************/
mssp_i2c_t mssp_i2c = {.i2c_clock=100000,.i2c_cfg.i2c_SMBus_control=I2C_SLEW_RATE_ENABLE,
.i2c_cfg.i2c_general_call=I2C_GENERAL_CALL_ENABLE,.i2c_cfg.i2c_master_rec_mode=I2C_MASTER_RECEIVE_DISABLE,
.i2c_cfg.i2c_mode=I2C_MSSP_MASTER_MODE,.i2c_cfg.i2c_mode_cfg=I2C_MASTER_MODE_DEFINED_CLOCK,
.I2C_DefaultInterruptHandler=NULL,.I2C_Report_Receive_Overflow=NULL,.I2C_Report_Write_Collision=NULL};
/********************************************* CCP1 ***********************************************************/
ccp1_t ccp1_={.CCP1_handler=NULL,.ccp1_mode=ccp1_pwm_initialize,.pin_config.port=PORTC_INDEX,.pin_config=pin2,
.pwm_frequence=5000,.timer2_postscaler_cfg=ccp1_Postscale_div_by_1,.timer2_prescaler_cfg=ccp1_prescale_div_by_1,
.pin_config.direction=GPIO_OUTPUT};
/********************************************* TIMER2 ***********************************************************/
Timer2_t Timer2_1 ={.Timer2_handler=NULL,
                    .timer1_preload_value=0,.timer2_postscaler_cfg=Postscale_div_by_1,
                    .timer2_prescaler_cfg=prescale_div_by_1};
/********************************************* LED ***********************************************************/
led_t led4={.port_name=PORTC_INDEX,.pin=pin5,.led_sta=led_off};
led_t led6={.port_name=PORTC_INDEX,.pin=pin7,.led_sta=led_off};
/********************************************* ULTRASONIC ***********************************************************/
ultra_pins_t ultra_pins = {.trigger_pin.port=PORTC_INDEX,.trigger_pin.pin=pin1,.ECHO_pin.direction=GPIO_OUTPUT,
                           .ECHO_pin.port=PORTC_INDEX,.ECHO_pin.pin=pin0,.ECHO_pin.direction=GPIO_INPUT};
/********************************************* PIN ***********************************************************/
pin_config_t buzzer ={.port=PORTC_INDEX,.pin=pin6,.direction=GPIO_OUTPUT,.logic=GPIO_LOW};
pin_config_t servo1 ={.port=PORTC_INDEX,.pin=pin2,.direction=GPIO_OUTPUT,.logic=GPIO_LOW};
/********************************************* TIMER1 ***********************************************************/
Timer1_t Timer1_1 ={.Timer1_handler=NULL,.timer1_mode=TIMER1_source_internal,
                    .timer1_osc_cfg=TIMER1_oscillator_disable,.timer1_preload_value=0,
                    .timer1_prescaler_cfg=prescale_value_timer1_div_by_1,.timer1_wr__reg_cfg=TIMER1_8bit_cfg};


// TC74
uint8 _ack;
uint8 ret_data_1;  //FOR RELAY
uint8 ret_data_2;  // MAIN TEMPERATURE
uint8 ret_data_3;  //MOTORS
// ULTRASONIC 
uint8 ultra_data;
uint8 ultra_data_int_;
// I2C
uint8 i2c_data;
uint8 i2c_ack;
// KEYPAD
uint8 data_key_1;
uint8 data_key_send;
Std_ReturnType ret ;
int main() {
    
    
    intitialized_functions();
    
    while(1)
    {
        /* read temperature  */
        tc74_read_value(&mssp_i2c,ADDRESS_A7,REDISTER_READ_0,&ret_data_1,&_ack);
        __delay_ms(5);
        tc74_read_value(&mssp_i2c,ADDRESS_A6,REDISTER_READ_0,&ret_data_2,&_ack);
        __delay_ms(5);
        tc74_read_value(&mssp_i2c,ADDRESS_A4,REDISTER_READ_0,&ret_data_3,&_ack);
        __delay_ms(5);
        
        /* send temp for relay_1 */
        if(ret_data_1 > 25){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'A',&_ack);
        }else{
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'C',&_ack);
        }
        
        /*  temp with motors */
        if ((ret_data_3 > 30)){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'K',&_ack);
        }else if ((ret_data_3 < 30) && (ret_data_3 >= 27)){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'L',&_ack);
        }else if ((ret_data_3 < 27) && (ret_data_3 >= 25)){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'H',&_ack);
        }else if ((ret_data_3 < 25)){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'G',&_ack);
        }
        /* ultrasonic with leds and servo motor  */
        ultra_data = distance_ultrasonic(&ultra_pins);
        ultra_data_int_ = (uint8)ultra_data;
        
        if(((ultra_data_int_) >= 80)){
            led_turn_off(&led4);
            led_turn_off(&led6);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_LOW);
            __delay_us(10);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_HIGH);
        }else if(((ultra_data_int_) < 80) && ((ultra_data_int_) >= 75)){
            led_turn_on(&led4);
            led_turn_off(&led6);
            
        }else if(((ultra_data_int_) < 75) && ((ultra_data_int_) >= 70)){
            led_turn_on(&led4);
            led_turn_on(&led6);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_LOW);
            __delay_us(10);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_HIGH);
        }else if(((ultra_data_int_) < 70) && ((ultra_data_int_) >= 67)){
            led_turn_on(&led4);
            led_turn_on(&led6);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_HIGH);
            __delay_us(10);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_LOW);
        }else if(((ultra_data_int_) < 67)){
            led_turn_off(&led4);
            led_turn_off(&led6);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_LOW);
            __delay_us(10);
            GPIO_pIN_WRITE_LOGIC(&servo1,GPIO_HIGH);
        }
        
        /* read temperature and check with buzzer */
        if(ret_data_2 >  40){
            while(ret_data_2 > 40){
                tc74_read_value(&mssp_i2c,ADDRESS_A6,REDISTER_READ_0,&ret_data_2,&_ack);
                MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,ret_data_2,&_ack);
                 __delay_ms(50);
                 GPIO_pIN_WRITE_LOGIC(&buzzer,GPIO_HIGH);
                 led_turn_off(&led4);
                 led_turn_off(&led6);
                 __delay_ms(100);
                 led_turn_on(&led4);
                 led_turn_on(&led6);
                 GPIO_pIN_WRITE_LOGIC(&buzzer,GPIO_LOW);
                 __delay_ms(100);
            }
                 led_turn_off(&led4);
                 led_turn_off(&led6);
                 GPIO_pIN_WRITE_LOGIC(&buzzer,GPIO_LOW);
        }
        /* I2C and keypad */
        keypad_get_value(&matrix_keypad,&data_key_1);
        if(data_key_1 == '1'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'Q',&_ack);
        }else if(data_key_1 == '2'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'W',&_ack);
        }else if(data_key_1 == '3'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'E',&_ack);
        }else if(data_key_1 == '4'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'R',&_ack);
        }else if(data_key_1 == '5'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'T',&_ack);
        }else if(data_key_1 == '6'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'Y',&_ack);
        }else if(data_key_1 == '7'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'U',&_ack);
        }else if(data_key_1 == '8'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'I',&_ack);
        }else if(data_key_1 == '9'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'O',&_ack);
        }else if(data_key_1 == '*'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'S',&_ack);
        }else if(data_key_1 == '*'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'S',&_ack);
        }else if(data_key_1 == '/'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'F',&_ack);
        }else if(data_key_1 == '-'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'J',&_ack);
        }else if(data_key_1 == '+'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'Z',&_ack);
        }else if(data_key_1 == '='){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'X',&_ack);
        }else if(data_key_1 == '0'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'V',&_ack);
        }else if(data_key_1 == '#'){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,'B',&_ack);
        }
        
        /*  temperature of main room  */
        if(ret_data_2 < 50){
            MSSP_I2C_Master_call_slave(&mssp_i2c,slave_address,ret_data_2,&_ack);
        }
        
    }
    return (EXIT_SUCCESS);
}


void intitialized_functions (void){
    ret = MSSP_I2C_Init(&mssp_i2c);
    Timer1_init(&Timer1_1);
    ultrasonic_initialize(&ultra_pins);
    led_intialize(&led4);
    led_intialize(&led6);
    GPIO_pIN_INTIALIZE(&buzzer);
    GPIO_pIN_INTIALIZE(&servo1);
    keypad_intialize(&matrix_keypad);
}