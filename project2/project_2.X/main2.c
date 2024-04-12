#include"main_2.h"
#include "ECU_layer/button/ecu_button.h"
#include "MKAL_layer/I2C/hal_i2c.h"


void timer1_callback_1(void);
void timer0_callback_1 (void);
void into_interrupt(void);
void int1_interrupt(void);
void interrupt_usart_rx(void);
void CCP1_handler_fun(void);
void CCP2_handler_fun(void);
 void mssp_I2C_DefaultInterruptHandler(void);
#define slave_address 0x60
/********************************************* ADC ***********************************************************/
ADC_conf_t ADC_ = {.ADC_CHANNAL_SELECT=ADC_CHANNEL_AN0,.ADC_interruptHandler=NULL,.Acquisition_time=ADC_12_TAD,
.Conversion_Clock=FOSC_div_16,.result_format=ADC_result_right,.voltage_ref=voltage_ref_disable};
/********************************************* INT ***********************************************************/
INTERRUPT_INTX_t INT_0 ={.EXT_INTERRUPTHANDLE =into_interrupt,.edge=INTERRUPT_RISING_EDGE,.priority=INTERRUPT_HUGH_PRIORITY,
.sorce=INTERRUPT_INT0,.pin_config.pin=pin0,.pin_config.port=PORTB_INDEX,.pin_config.direction=GPIO_INPUT,.pin_config.logic=GPIO_LOW};
INTERRUPT_INTX_t INT_1 ={.EXT_INTERRUPTHANDLE =int1_interrupt,.edge=INTERRUPT_RISING_EDGE,.priority=INTERRUPT_HUGH_PRIORITY,
.sorce=INTERRUPT_INT1,.pin_config.pin=pin1,.pin_config.port=PORTB_INDEX,.pin_config.direction=GPIO_INPUT,.pin_config.logic=GPIO_LOW};
/********************************************* CCP ***********************************************************/
ccp1_t ccp1_={.CCP1_handler=NULL,.ccp1_mode=ccp1_pwm_mode_select,.ccp1_mode_variant=ccp1_pwm_initialize,.pin_config.port=PORTC_INDEX
,.pin_config.pin=pin2,.pin_config.direction=GPIO_OUTPUT,.timer2_postscaler_cfg=ccp1_Postscale_div_by_1,
.timer2_prescaler_cfg=ccp1_prescale_div_by_1,.pwm_frequence=0};
/********************************************* timer2 ***********************************************************/
Timer2_t Timer2 ={.Timer2_handler=NULL,
                    .timer1_preload_value=0,.timer2_postscaler_cfg=Postscale_div_by_1,
                    .timer2_prescaler_cfg=prescale_div_by_1};
/********************************************* timer3 ***********************************************************/
Timer3_t Timer3_1={.Timer3_handler=NULL,.timer3_mode=TIMER3_source_internal,.timer3_preload_value=0,
                   .timer3_prescaler_cfg=Prescale_value_div_1,.timer3_wr__reg_cfg=TIMER3_8bit_cfg};
/********************************************* MOTORS DC ***********************************************************/
motor_t motor_1={.motor_pin_config[0].port=PORTC_INDEX,.motor_pin_config[0].pin=pin0,.motor_pin_config[0].direction=GPIO_OUTPUT
,.motor_pin_config[1].port=PORTC_INDEX,.motor_pin_config[1].pin=pin5,.motor_pin_config[1].direction=GPIO_OUTPUT};
motor_t motor_2={.motor_pin_config[0].port=PORTC_INDEX,.motor_pin_config[0].pin=pin6,.motor_pin_config[0].direction=GPIO_OUTPUT
,.motor_pin_config[1].port=PORTC_INDEX,.motor_pin_config[1].pin=pin7,.motor_pin_config[1].direction=GPIO_OUTPUT};
/********************************************* I2C ***********************************************************/
mssp_i2c_t mssp_i2c={.i2c_clock=100000,.i2c_cfg.i2c_SMBus_control=I2C_SLEW_RATE_ENABLE,.i2c_cfg.i2c_general_call=I2C_GENERAL_CALL_ENABLE
,.i2c_cfg.i2c_mode=I2C_MSSP_SLAVE_MODE,.i2c_cfg.i2c_slave_address=slave_address,.i2c_cfg.i2c_slew_rate=I2C_SLEW_RATE_ENABLE,
.i2c_cfg.i2c_mode_cfg=I2C_SLAVE_MODE_7BIT_ADDRESS,.I2C_DefaultInterruptHandler=mssp_I2C_DefaultInterruptHandler};
/********************************************* LCD ***********************************************************/
chr_lcd_4bit_t lcd_4bit = {.lcd_en.port=PORTD_INDEX,.lcd_en.pin=pin0,.lcd_en.direction=GPIO_OUTPUT,.lcd_en.logic=GPIO_LOW,
.lcd_rs.port=PORTD_INDEX,.lcd_rs.pin=pin5,.lcd_rs.direction=GPIO_OUTPUT,.lcd_rs.logic=GPIO_LOW,
.lcd_data[0].port=PORTD_INDEX,.lcd_data[0].pin=pin1,.lcd_data[0].direction=GPIO_OUTPUT,.lcd_data[0].logic=GPIO_LOW,
.lcd_data[1].port=PORTD_INDEX,.lcd_data[1].pin=pin2,.lcd_data[1].direction=GPIO_OUTPUT,.lcd_data[1].logic=GPIO_LOW,
.lcd_data[2].port=PORTD_INDEX,.lcd_data[2].pin=pin3,.lcd_data[2].direction=GPIO_OUTPUT,.lcd_data[2].logic=GPIO_LOW,
.lcd_data[3].port=PORTD_INDEX,.lcd_data[3].pin=pin4,.lcd_data[3].direction=GPIO_OUTPUT,.lcd_data[3].logic=GPIO_LOW};
/********************************************* LCD ***********************************************************/
led_t led3={.port_name=PORTD_INDEX,.pin=pin6,.led_sta=led_off};
led_t led5={.port_name=PORTD_INDEX,.pin=pin7,.led_sta=led_off};
/********************************************* PINS ***********************************************************/
pin_config_t relay={.port=PORTC_INDEX,.pin=pin1,.direction=GPIO_OUTPUT,.logic=GPIO_LOW};

uint16 value_1 ;
uint8 l_direction;
uint8 ccc;
uint8 ddc;
uint8 counter;

// i2c
uint8 i2c_ack;
volatile uint8 i2c_data;
volatile uint8 i2c_data_1;
volatile uint8 i2c_data_2;
volatile uint8 i2c_data_3;
volatile uint8 i2c_data_4;
volatile uint8 counter_;
// keypad
uint8 key_data_1;
uint8 key_data_2;
uint8 key_i2c_tran;
// string convert
uint8 string[6];


Std_ReturnType ret;


int main() {
    intitialized_functions();
    while(1)
    {
        /* SERVO MOTOR CONTROL */
        ADC_get_conversion_blocking(&ADC_,ADC_CHANNEL_AN0,&value_1);
        l_direction = (uint32)100 * value_1 /1022 ;
        if(l_direction == 0){
            ccp1_pwm_set_stop();
        }else{
            ccp1_pwm_set_duty(l_direction);
            ccp1_pwm_start();
        }
        
        /* receive from keypad  */
        
        /* TEMP receive from master to initialize relay */
        if(i2c_data_1 == 'A'){
            counter_++;
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_HIGH);
        }else if(i2c_data_1 == 'C'){
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_LOW);
        }
        if(i2c_data_2 == 'K'){
            morot_turn_right(&motor_1);
            morot_turn_left(&motor_2);
        }else if(i2c_data_2 == 'H'){
            morot_turn_left(&motor_1);
            morot_stop(&motor_2);
        }else if(i2c_data_2 == 'L'){
            morot_stop(&motor_1);
            morot_turn_left(&motor_2);
        }else if(i2c_data_2 == 'G'){
            morot_stop(&motor_1);
            morot_stop(&motor_2);
        }
        
        
        /* receive from keypad to control */
        if(i2c_data_3 == 'Q'){ //1
            led_turn_on(&led3);
        }else if(i2c_data_3 == 'W'){  //2
            led_turn_off(&led3);
        }else if(i2c_data_3 == 'E'){  //3
            led_turn_on(&led5);;
        }
        else if(i2c_data_3 == 'R'){  //4
            led_turn_off(&led5);
        }
        else if(i2c_data_3 == 'T'){  //5
            morot_turn_right(&motor_2);
        }
        else if(i2c_data_3 == 'Y'){  //6
            morot_stop(&motor_2);
        }
        else if(i2c_data_3 == 'U'){  //7
            morot_turn_right(&motor_1);
        }
        else if(i2c_data_3 == 'I'){  //8
            morot_stop(&motor_1);
        }
        else if(i2c_data_3 == 'O'){  //9
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_HIGH);
        }
        else if(i2c_data_3 == 'P'){  //*
            
        }
        else if(i2c_data_3 == 'S'){  ///
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_LOW);
        }else if(i2c_data_3 == 'F'){  //-
            
        }else if(i2c_data_3 == 'J'){  //*
            
        }
        else if(i2c_data_3 == 'Z'){  //=
            
        }else if(i2c_data_3 == 'X'){  //=
            
        }else if(i2c_data_3 == 'V'){  //0
            lcd_4bit_send_command(&lcd_4bit,_LCD_DISPLAY_OFF_CURSOR_OFF);
            led_turn_off(&led3);
            led_turn_off(&led5);
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_LOW);
            morot_stop(&motor_1);
            morot_stop(&motor_2);
        }else if(i2c_data_3 == 'B'){  //#
            lcd_4bit_send_command(&lcd_4bit,_LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
            led_turn_on(&led3);
            led_turn_on(&led5);
            GPIO_pIN_WRITE_LOGIC(&relay,GPIO_HIGH);
            morot_turn_right(&motor_1);
            morot_turn_left(&motor_2);
        }
        
        /* LCD temperature */
        if(i2c_data_4 <= 50){
            convert_uint8_to_string(i2c_data_4,string);
            lcd_4bit_send_string_pos(&lcd_4bit,1,2,"Temperature :");
            lcd_4bit_send_string_pos(&lcd_4bit,1,16,string);
            if((i2c_data_4 <= 50)&& (i2c_data_4 > 40)){
                lcd_4bit_send_string_pos(&lcd_4bit,2,2,"Error !!!!!    ");
                led_toggle(&led3);
                led_toggle(&led5);
                lcd_4bit_send_command(&lcd_4bit,_LCD_DISPLAY_OFF_CURSOR_OFF);
                __delay_ms(50);
                lcd_4bit_send_command(&lcd_4bit,_LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
            }else if((i2c_data_4 <= 40) && (i2c_data_4 >= 36)){
                lcd_4bit_send_string_pos(&lcd_4bit,2,2,"dangerous temp !");
            }else if((i2c_data_4 < 36) && (i2c_data_4 >= 34)){
                if(counter_ == 0){
                   lcd_4bit_send_string_pos(&lcd_4bit,2,2,"                 ");
                  counter_=1;
                }
                
                lcd_4bit_send_string_pos(&lcd_4bit,2,2,"Warning temp !");
            }else{
                lcd_4bit_send_string_pos(&lcd_4bit,2,2,"Normal statue     ");
            }
        }

    }
    return (EXIT_SUCCESS);
}

void intitialized_functions (void){
    MSSP_I2C_Init(&mssp_i2c);
    ADC_inti(&ADC_);
    Interrupt_Intx_Inti(&INT_0);
    Interrupt_Intx_Inti(&INT_1);
    motor_intialize(&motor_1);
    motor_intialize(&motor_2);
    ccp1_init(&ccp1_);
    Timer2_init(&Timer2);
    Timer3_init(&Timer3_1);
    GPIO_pIN_INTIALIZE(&relay);
    lcd_4bit_intialize(&lcd_4bit);
    led_intialize(&led3);
    led_intialize(&led5);
}


void timer1_callback_1(void){
    
}

void timer0_callback_1 (void){
    
}

void into_interrupt(void){
    
}

void int1_interrupt(void){
    
}

void interrupt_usart_rx(void){
    
}

void CCP1_handler_fun(void){
    
}

void CCP2_handler_fun(void){
    
}

 void mssp_I2C_DefaultInterruptHandler(void){
    // holds clock low (clock stretch)
     
    I2C_CLOCK_STRETCH_ENABLE();
    if((SSPSTATbits.D_nA == 0) && (SSPSTATbits.R_NOT_W == 0)){
        uint8 dummy_buffer =SSPBUF;
        while(!SSPSTATbits.BF);
        i2c_data =SSPBUF;
    }
    I2C_CLOCK_STRETCH_DISABLE();
    if((i2c_data == 'A') || (i2c_data == 'C')){
         i2c_data_1 = i2c_data;
    }else if((i2c_data == 'K') || (i2c_data == 'L') || (i2c_data == 'H') || (i2c_data == 'G')){
         i2c_data_2 = i2c_data;
    }else if((i2c_data == 'Q') || (i2c_data == 'W') || (i2c_data == 'E') || (i2c_data == 'R') ||
             (i2c_data == 'T') || (i2c_data == 'Y') || (i2c_data == 'U') || (i2c_data == 'I') ||
             (i2c_data == 'O') || (i2c_data == 'P') || (i2c_data == 'S') || (i2c_data == 'D') ||
             (i2c_data == 'F') || (i2c_data == 'J') || (i2c_data == 'Z') || (i2c_data == 'X') ||
             (i2c_data == 'V') || (i2c_data == 'B')){
         i2c_data_3 = i2c_data;
    }else if(i2c_data <= 50){
        i2c_data_4 = i2c_data;
    }
}