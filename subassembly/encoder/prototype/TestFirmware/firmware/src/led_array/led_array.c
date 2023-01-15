
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "led_array.h"

/*
 * Prototype Board:
 * A - PA02
 * B - PA03
 * C - PA06
 * D - PA05
 * E - PA04
 * RIB v1:
 * A - PA03
 * B - PA04
 * C - PA07
 * D - PA06
 * E - PA05
 */



//BCM Cycles - How many brightness levels do we support?
#define LED_CYCLES 4
//How many ticks do we want each cycle to last?  Allows for brightness control.
static uint8_t cycle_ticks[LED_CYCLES] = { 1, 2, 8, 16};

//Individual data phases - Repeated for each cycle.  Each phase
//is one driven pin and 0-n pins low
#define LED_PHASES 5

#define LED_CYCLE_PHASES (LED_CYCLES * LED_PHASES)

#define LED_A_PHASE 0
#define LED_B_PHASE 1
#define LED_C_PHASE 2
#define LED_D_PHASE 3
#define LED_E_PHASE 4

// Indicates port offset
#define LED_A_OFFSET 2
#define LED_B_OFFSET 3
#define LED_C_OFFSET 6
#define LED_D_OFFSET 5
#define LED_E_OFFSET 4

//const static uint32_t LED_MASK =
//                0x01 << LED_A_OFFSET |
//                0x01 << LED_B_OFFSET |
//                0x01 << LED_C_OFFSET |
//                0x01 << LED_D_OFFSET |
//                0x01 << LED_E_OFFSET ;

const static uint32_t LED_MASK_INV = ~(  
                0x01 << LED_A_OFFSET |
                0x01 << LED_B_OFFSET |
                0x01 << LED_C_OFFSET |
                0x01 << LED_D_OFFSET |
                0x01 << LED_E_OFFSET) ;


const static uint8_t led_pins[LED_ARRAY_COUNT][2] = {
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

// Used to assign OUTCLR in each cycle phase.
static uint8_t signals[LED_CYCLE_PHASES];
// Used to assign PORT_DIR in each cycle phase.
static uint8_t directions[LED_CYCLE_PHASES];

// In each cycle, 
const static uint8_t LED_PHASE_FEED[LED_PHASES] = {
    0x01 << LED_A_OFFSET,
    0x01 << LED_B_OFFSET,
    0x01 << LED_C_OFFSET,
    0x01 << LED_D_OFFSET,
    0x01 << LED_E_OFFSET,
};

static port_group_registers_t *LED_PORT = &(PORT_REGS->GROUP[0]);
static uint8_t phase = 0;
static uint8_t tick_counter = 0;
static uint8_t cycle = 0;

void led_array_init() {
    for(int cycle_phase = 0; cycle_phase < LED_CYCLE_PHASES; cycle_phase++) {
        signals[cycle_phase] = LED_PHASE_FEED[cycle_phase % LED_PHASES];
        directions[cycle_phase] = LED_PHASE_FEED[cycle_phase % LED_PHASES];
    }
    
    //Initialize for phase 0;
    phase = 0;
    cycle = 0;
    tick_counter = 0;
    
    LED_PORT->PORT_DIR = (LED_PORT->PORT_DIR & LED_MASK_INV) | directions[phase];
}

void led_array_phase() {
    int16_t cycle_phase_offset = phase + (cycle * LED_PHASES);
    LED_PORT->PORT_OUTCLR = signals[cycle_phase_offset];
    LED_PORT->PORT_DIRCLR = directions[cycle_phase_offset];
    
    phase++;
    
    if(phase >= LED_PHASES) {
        phase = 0;
        cycle++;
        
        if(cycle >= LED_CYCLES) {
            cycle = 0;
        }
    }
    
    cycle_phase_offset = phase + (cycle * LED_PHASES);
    
    LED_PORT->PORT_OUTSET = signals[cycle_phase_offset];
    LED_PORT->PORT_DIRSET = directions[cycle_phase_offset];
}

void led_array_tick() {
    tick_counter++;
    
    if(tick_counter >= cycle_ticks[cycle]) {
        led_array_phase();
        tick_counter = 0;
    }
}

void led_array_set_led(uint8_t position, uint8_t value) {
    uint8_t phase = led_pins[position][0];
    uint8_t flag = led_pins[position][1];
    
    //Bit shift things down to deal with reduced space
    value = value >> (8 - LED_CYCLES);
    
    for(int8_t cycle_counter = 0 ; cycle_counter < LED_CYCLES; cycle_counter++) {  
        int16_t cycle_phase_offset = phase + (cycle_counter * LED_PHASES);
        
        //Check the LED `value` against the cycle counter.
        if(value & (0x01 << cycle_counter)) {
            //LED is turned on by sinking current as an output pin
            directions[cycle_phase_offset] = directions[cycle_phase_offset] | flag;
        } else {
            //LED is turned off by going high impedance / input
            directions[cycle_phase_offset] = directions[cycle_phase_offset] & ~flag;
        }
    }
}
