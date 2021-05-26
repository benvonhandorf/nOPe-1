/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "encoder/encoder.h"

#include "led_array/led_array.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
#define ANIMATION_TYPES 3

uint8_t mode = 0;
int8_t led_focused = 0;
int8_t type = 0;
int16_t speed = 1;
uint32_t counter = 0;
const uint8_t tail = 2;

void process_init() {
    for(uint8_t i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
    
    led_focused = 0;
    
    led_array_set_led(led_focused, 0xFF);
}

void process_switch_mode() {
    mode = !mode;
    
    for(uint8_t i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
}

void process_type_0(int32_t encoder_data) {
    
    if(encoder_data) {
        speed += encoder_data;
    }
    
    for(uint8_t i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
    
    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;
    
    led_focused += direction;
    
    while(led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while(led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }

    for(int8_t i = 0; i < tail; i++) {
        int8_t led = led_focused + (i * direction);
        
        while(led < 0) {
            led += LED_ARRAY_COUNT;
        }
        
        while(led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }
        
        if(i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }
}

void process_type_1(int32_t encoder_data) {
    if(encoder_data) {
        speed += encoder_data;
    }
    
    for(uint8_t i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
    
    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;
    
    led_focused += direction;
    
    while(led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while(led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }

    for(uint8_t i = 0; i < tail; i++) {
        int8_t led = led_focused + (i * direction);
        
        while(led < 0) {
            led += LED_ARRAY_COUNT;
        }
        
        while(led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }
        
        if(i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }
    
    for(uint8_t i = 0; i < tail; i++) {
        int8_t led = led_focused - (i * direction);
        
        while(led < 0) {
            led += LED_ARRAY_COUNT;
        }
        
        while(led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }
        
        if(i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }
}

int8_t led_sweep = 0;

void process_type_2(int32_t encoder_data) {
    if(encoder_data) {
        speed += encoder_data;
    }
    
    for(uint8_t i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
    
    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;
    
    led_sweep++;
    
    if(led_sweep >= (2 * LED_ARRAY_COUNT)
            || led_sweep <= -(2 * LED_ARRAY_COUNT)) {
        //After sweeping positively and then negatively, clear
        led_focused += direction;
        led_sweep = 0;
    }
    
    while(led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while(led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }
    
    for(uint8_t i = 0; i < led_sweep; i++) {
        int8_t led = led_focused + (i * direction);
        
        while(led < 0) {
            led += LED_ARRAY_COUNT;
        }
        
        while(led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }
        
        if(i < LED_ARRAY_COUNT) {
            led_array_set_led((uint8_t) led, 0x20);
        } else {
            led_array_set_led((uint8_t) led, 0);
        }
    }
    
    led_array_set_led((uint8_t) led_focused, 0xFF);
 }

void process_normal(int32_t encoder_data) {
    counter++;
    
    int8_t abs_speed = speed == 0 ? 1 : speed > 0 ? speed : -speed;
    
    if((counter * abs_speed) >= 100000) {
        counter = 0;
        switch(type) {
            case 0:
                process_type_0(encoder_data);
                break;
            case 1:
                process_type_1(encoder_data);
                break;
            case 2:
                process_type_2(encoder_data);
                break;
        }
    }
}

void process_mode_adjustment(int32_t encoder_data) {
    if(encoder_data) {
        type += encoder_data;
        
        while(type < 0) {
            type += ANIMATION_TYPES;
        } 
        while(type >= ANIMATION_TYPES) {
            type -= ANIMATION_TYPES;
        }
    }
    
    for(int i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }
    
    led_array_set_led(type, 0xFF);
}

void process_mode(int32_t encoder_data) {
    switch(mode) {
        case 0:
            process_normal(encoder_data);

            break;
        case 1:
            process_mode_adjustment(encoder_data);

            break;
    }
}

static port_group_registers_t *COMMS_PORT = &(PORT_REGS->GROUP[0]);
//TODO: Make address configurable.  Jumpers on board?
//Addresses: 
//BASE + 0 - Value offset, r/w
//BASE + 1 - Count of clicks, r/w
//BASE + 3 - Illumination Type, write only
//BASE + 4 - Illumination Data - Up to 20 Bytes, non provided bytes will be treated as zeros, write only
#define I2C_ADDRESS 0x12

static sercom_i2cs_registers_t *I2CS_PORT = (sercom_i2cs_registers_t *) SERCOM0_BASE_ADDRESS;

void comms_init() {
    COMMS_PORT->PORT_DIRSET  |= 1 << PIN_PA16;
    I2CS_PORT->SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE;
    I2CS_PORT->SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(I2C_ADDRESS) | SERCOM_I2CS_ADDR_ADDRMASK(0x7F);
    I2CS_PORT->SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_AMODE(0); //MASK mode
    I2CS_PORT->SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE(1);
    I2CS_PORT->SERCOM_INTENSET |= SERCOM_I2CS_INTENSET_DRDY_Msk ;
}

uint8_t comms_addr = 0x00;
// 1 - Central Out, Peripheral In
// 2 - Central In, Peripheral Out
uint8_t comms_state = 0x00; 
uint8_t clicks = 0;
int32_t data_value = 0;
#define COMMS_BUFF_SIZE 20
uint8_t comms_buffer[COMMS_BUFF_SIZE];
uint8_t comms_buffer_offset = 0;

void comms_update(uint8_t new_clicks, int32_t encoder_data) {
    clicks += new_clicks;
    data_value += encoder_data;
    
//    if(comms_state) {
//        printf("ADDR: %x", comms_addr);
//    }
    
    if(clicks || data_value) {
        // Data that is yet to be read exists.  Notify the controller
//        printf("Data: %d, %d", clicks, data_value);
        COMMS_PORT->PORT_OUTSET |= 1 << PIN_PA16;
    } else {
        COMMS_PORT->PORT_OUTCLR |= 1 << PIN_PA16;
    }
}

void SERCOM0_I2C_InterruptHandler() {
    if(I2CS_PORT->SERCOM_INTFLAG & SERCOM_I2CS_INTENSET_DRDY_Msk ) {
        comms_buffer[comms_buffer_offset++] = I2CS_PORT->SERCOM_DATA;
    } else if(I2CS_PORT->SERCOM_INTFLAG & SERCOM_I2CS_INTENSET_AMATCH_Msk ) {
        comms_addr = I2CS_PORT->SERCOM_DATA;
        
        //Clear the match interrupt
        I2CS_PORT->SERCOM_INTFLAG |= SERCOM_I2CS_INTENSET_AMATCH_Msk;
        
        if(I2CS_PORT->SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) {
            //Peripheral read operation
            comms_state = 1;
        } else {
            //Peripheral write operation
            comms_state = 2;
        }
        
    }
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    led_array_init();
    
    //EIC is configured by the encoder
    encoder_init();
    
    process_init();
        
    NVIC_INT_Enable();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        led_array_tick();
        
        encoder_tick();
        
        uint8_t clicks = encoder_get_clicks();
        int32_t encoder_data = encoder_get_increment();
        
        comms_update(clicks, encoder_data);
        
//        if(clicks) {
//            process_switch_mode();
//        }
//        
//        process_mode(encoder_data);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

