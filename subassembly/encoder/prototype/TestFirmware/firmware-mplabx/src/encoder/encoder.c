
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "encoder.h"


volatile static uint8_t clicks = 0;
volatile int8_t increment = 0;

volatile uint32_t increment_lockout = 0;

/*
 * Prototype Board:
 * A - PA10
 * B - PA07
 * RIB v1:
 * A - PA11
 * B - PA10
 */

void encoder_init() {
    EIC_REGS->EIC_CONFIG[0] =
            //                        EIC_CONFIG_FILTEN7(1) | 
            EIC_CONFIG_SENSE7_FALL | //Filter and falling edge detection for EIC 7 - ENC_SW
            //                        EIC_CONFIG_FILTEN3(1) | 
            EIC_CONFIG_SENSE3_FALL | EIC_CONFIG_SENSE3_RISE; //Filter and falling edge detection for EIC 3 - ENC_A

    EIC_REGS->EIC_INTENSET = EIC_INTENSET_EXTINT3(1) |
            EIC_INTENSET_EXTINT7(1);

    EIC_REGS->EIC_CTRL = EIC_CTRL_ENABLE(1);

    NVIC_EnableIRQ(EIC_IRQn);
}

uint8_t encoder_state = 0x00;
int8_t transition_validity[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

void EIC_Handler() {
    if(!increment_lockout) {
        increment_lockout = 10000;
        if(EIC_REGS->EIC_INTFLAG & EIC_INTFLAG_EXTINT3(1)) {
            //ENC_A
            if(PORT_REGS->GROUP[0].PORT_IN & (0x01 << 10)) {
                increment = -1;
            } else {
                increment = 1;
            }
        } else if(EIC_REGS->EIC_INTFLAG & EIC_INTFLAG_EXTINT7(1)) {
            increment_lockout = 10000;
            //ENC_SW
            clicks++;
        }
    }

    EIC_REGS->EIC_INTFLAG = EIC_INTFLAG_EXTINT3(1) | EIC_INTFLAG_EXTINT7(1);
}

void encoder_tick() {
    if (increment_lockout) {
        increment_lockout--;
    }
}

int32_t encoder_get_increment() {
    int32_t result = increment;
    increment -= result;

    return result;
}

uint8_t encoder_get_clicks() {
    uint8_t result = clicks;
    clicks -= result;

    return result;
}


