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


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/*
 * A - PA02
 * B - PA03
 * C - PA06
 * D - PA05
 * E - PA04
 */

#define LED_COUNT 20

#define LED_PHASES 5

#define LED_A_PHASE 0
#define LED_B_PHASE 1
#define LED_C_PHASE 2
#define LED_D_PHASE 3
#define LED_E_PHASE 4

#define LED_A_OFFSET 2
#define LED_B_OFFSET 3
#define LED_C_OFFSET 6
#define LED_D_OFFSET 5
#define LED_E_OFFSET 4

const uint32_t LED_MASK =
                0x01 << LED_A_OFFSET |
                0x01 << LED_B_OFFSET |
                0x01 << LED_C_OFFSET |
                0x01 << LED_D_OFFSET |
                0x01 << LED_E_OFFSET ;
const uint32_t LED_MASK_INV = ~(  
                0x01 << LED_A_OFFSET |
                0x01 << LED_B_OFFSET |
                0x01 << LED_C_OFFSET |
                0x01 << LED_D_OFFSET |
                0x01 << LED_E_OFFSET) ;


uint8_t led_pins[][2] = {
    {LED_A_PHASE, 0x01 << LED_B_OFFSET}, // D1 - Column A, Row B
    {LED_A_PHASE, 0x01 << LED_C_OFFSET}, // D2 - Column A, Row C
    {LED_A_PHASE, 0x01 << LED_D_OFFSET}, // D3 - Column A, Row D
    {LED_A_PHASE, 0x01 << LED_E_OFFSET}, // D4 - Column A, Row E
    {LED_B_PHASE, 0x01 << LED_A_OFFSET}, // D5 - Column B, Row A
    {LED_B_PHASE, 0x01 << LED_C_OFFSET}, // D6 - Column B, Row C
    {LED_B_PHASE, 0x01 << LED_D_OFFSET}, // D7 - Column B, Row D
    {LED_B_PHASE, 0x01 << LED_E_OFFSET}, // D8 - Column B, Row E
    {LED_C_PHASE, 0x01 << LED_A_OFFSET}, // D9 - Column C, Row A
    {LED_C_PHASE, 0x01 << LED_B_OFFSET}, // D10 - Column C, Row B
    {LED_C_PHASE, 0x01 << LED_D_OFFSET}, // D11 - Column C, Row D
    {LED_C_PHASE, 0x01 << LED_E_OFFSET}, // D12 - Column C, Row E
    {LED_D_PHASE, 0x01 << LED_A_OFFSET}, // D13 - Column D, Row A
    {LED_D_PHASE, 0x01 << LED_B_OFFSET}, // D14 - Column D, Row B
    {LED_D_PHASE, 0x01 << LED_C_OFFSET}, // D15 - Column D, Row C
    {LED_D_PHASE, 0x01 << LED_E_OFFSET}, // D16 - Column D, Row E
    {LED_E_PHASE, 0x01 << LED_A_OFFSET}, // D17 - Column E, Row A
    {LED_E_PHASE, 0x01 << LED_B_OFFSET}, // D18 - Column E, Row B
    {LED_E_PHASE, 0x01 << LED_C_OFFSET}, // D19 - Column E, Row C
    {LED_E_PHASE, 0x01 << LED_D_OFFSET}, // D20 - Column E, Row D
};

uint8_t signals[LED_PHASES];
uint8_t directions[LED_PHASES];

uint8_t LED_PHASE_FEED[LED_PHASES] = {
    0x01 << LED_A_OFFSET,
    0x01 << LED_B_OFFSET,
    0x01 << LED_C_OFFSET,
    0x01 << LED_D_OFFSET,
    0x01 << LED_E_OFFSET,
};

uint8_t phase = 0;

port_group_registers_t *LED_PORT = &(PORT_REGS->GROUP[0]);

void configure_phases() {
    for(int i = 0; i < LED_PHASES; i++) {
        signals[i] = LED_PHASE_FEED[i];
        directions[i] = LED_PHASE_FEED[i];
    }
    
    //Initialize for phase 0;
    phase = 0;
    
    LED_PORT->PORT_DIR = (LED_PORT->PORT_DIR & LED_MASK_INV) | directions[phase];
}

void next_phase() {
    LED_PORT->PORT_OUTCLR = signals[phase];
    LED_PORT->PORT_DIRCLR = directions[phase];
    
    phase++;
    
    if(phase >= LED_PHASES) {
        phase = 0;
    }
    
    LED_PORT->PORT_OUTSET = signals[phase];
    LED_PORT->PORT_DIRSET = directions[phase];
}

void set_led(uint8_t position, uint8_t on) {
    uint8_t phase = led_pins[position][0];
    uint8_t flag = led_pins[position][1];
    
    if(on) {
        //LED is turned on by sinking current as an output pin
        directions[phase] = directions[phase] | flag;
    } else {
        //LED is turned off by going high impedance - Input
        directions[phase] = directions[phase] & ~flag;
    }
}

volatile int8_t increment = 0;

void EIC_Handler() {
    if(EIC_REGS->EIC_INTFLAG & EIC_INTFLAG_EXTINT3(1)) {
        //ENC_A
        if(PORT_REGS->GROUP[0].PORT_IN & (0x01 << 10)) {
            increment = -1;
        } else {
            increment = 1;
        }
    } else if(EIC_REGS->EIC_INTFLAG & EIC_INTFLAG_EXTINT7(1)) {
        //ENC_SW
        increment += 5;
    }
    
    EIC_REGS->EIC_INTFLAG = EIC_INTFLAG_EXTINT3(1) | EIC_INTFLAG_EXTINT7(1);
}

int main ( void )
{
    uint8_t led = 0;
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    configure_phases();
    
    for(int i = 0; i < LED_COUNT; i++) {
        set_led(i, 0);
    }
        
    set_led(led, 1);
    
    EIC_REGS->EIC_CONFIG[0] =
            EIC_CONFIG_FILTEN7(1) | EIC_CONFIG_SENSE7_FALL | //Filter and falling edge detection for EIC 7 - ENC_SW
            EIC_CONFIG_FILTEN3(1) | EIC_CONFIG_SENSE3_FALL ; //Filter and falling edge detection for EIC 3 - ENC_A
    
    EIC_REGS->EIC_INTENSET = EIC_INTENSET_EXTINT3(1) |
            EIC_INTENSET_EXTINT7(1) ;
    
    EIC_REGS->EIC_CTRL = EIC_CTRL_ENABLE(1);
    
    NVIC_EnableIRQ(EIC_IRQn);
    
    NVIC_INT_Enable();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        next_phase();
        
        if(increment != 0) {
            int8_t hold = increment;
            int8_t new_led = led + hold;
            increment -= hold;
            
            while(new_led >= LED_COUNT) {
                new_led -= LED_COUNT;
            }
            
            while(new_led < 0) {
                new_led += LED_COUNT;
            }

            set_led(led, 0);
            
            led = new_led;
            
            set_led(led, 1);
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

