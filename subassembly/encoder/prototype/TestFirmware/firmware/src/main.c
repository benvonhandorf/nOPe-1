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

void process_type_0() {
    int32_t encoder = encoder_get_increment();
    
    if(encoder) {
        speed += encoder;
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

void process_type_1() {
    int32_t encoder = encoder_get_increment();
    
    if(encoder) {
        speed += encoder;
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

void process_type_2() {
    int32_t encoder = encoder_get_increment();
    
    if(encoder) {
        speed += encoder;
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
            led_array_set_led((uint8_t) led, 127);
        } else {
            led_array_set_led((uint8_t) led, 0);
        }
    }
    
    led_array_set_led((uint8_t) led_focused, 255);
 }

void process_normal() {
    counter++;
    
    int8_t abs_speed = speed == 0 ? 1 : speed > 0 ? speed : -speed;
    
    if((counter * abs_speed) >= 100000) {
        counter = 0;
        switch(type) {
            case 0:
                process_type_0();
                break;
            case 1:
                process_type_1();
                break;
            case 2:
                process_type_2();
                break;
        }
    }
}

void process_mode_adjustment() {
    int32_t encoder = encoder_get_increment();
    
    if(encoder) {
        type += encoder;
        
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

void process_mode() {
    switch(mode) {
        case 0:
            process_normal();

            break;
        case 1:
            process_mode_adjustment();

            break;
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
        
        if(clicks) {
            process_switch_mode();
        }
        
        process_mode();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

