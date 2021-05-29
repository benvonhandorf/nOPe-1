
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "comms.h"

#include "string.h"

//static port_group_registers_t *COMMS_PORT = &(PORT_REGS->GROUP[0]);
//TODO: Make address configurable.  Jumpers on board?
//Addresses: 
//BASE + 0 - Value offset, r/w
//BASE + 1 - Count of clicks, r/w
//BASE + 3 - Illumination Type, write only
//BASE + 4 - Illumination Data - Up to 20 Bytes, non provided bytes will be treated as zeros, write only
#define I2C_ADDRESS 0x12

static sercom_i2cs_registers_t *I2CS_PORT = (sercom_i2cs_registers_t *) SERCOM0_BASE_ADDRESS;

void comms_init() {
//    COMMS_PORT->PORT_DIRSET  |= PORT_DIRSET_DIRSET(1) << 16;
//    COMMS_PORT->PORT_OUTCLR  |= PORT_OUTCLR_OUTCLR(1) << 16;
    
    I2CS_PORT->SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE;
    I2CS_PORT->SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(I2C_ADDRESS) 
            | SERCOM_I2CS_ADDR_ADDRMASK(0x7F);
    I2CS_PORT->SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_AMODE(0); //MASK mode
    I2CS_PORT->SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE(1);
    I2CS_PORT->SERCOM_INTENSET |= SERCOM_I2CS_INTENSET_DRDY_Msk 
            | SERCOM_I2CS_INTENSET_AMATCH_Msk
            | SERCOM_I2CS_INTENSET_ERROR_Msk ;
}

static uint8_t comms_addr = 0x00;
// 1 - Central In, Peripheral Out
// 2 - Central Out, Peripheral In
static uint8_t comms_state = 0x00; 
static uint32_t clicks = 0;
static int32_t data_value = 0;
#define COMMS_BUFF_SIZE 20
static uint8_t comms_buffer[COMMS_BUFF_SIZE];
static uint8_t comms_buffer_offset = 0;
static uint8_t comms_buffer_size = 0;

void comms_update(uint32_t new_clicks, int32_t encoder_data) {
    clicks += new_clicks;
    data_value += encoder_data;
    
//    if(comms_state) {
//        printf("ADDR: %x", comms_addr);
//    }
    
//    if(clicks || data_value) {
//        // Data that is yet to be read exists.  Notify the controller
////        printf("Data: %d, %d", clicks, data_value);
//        COMMS_PORT->PORT_OUTSET |= 0x1 << PIN_PA16;
//    } else {
//        COMMS_PORT->PORT_OUTCLR |= 0x1 << PIN_PA16;
//    }
}

void comms_get_command(uint32_t *command, uint8_t *data) {
    *command = comms_addr;
    memcpy(data, comms_buffer, comms_buffer_size);
}

void SERCOM0_Handler() {
    if(I2CS_PORT->SERCOM_INTFLAG & SERCOM_I2CS_INTENSET_DRDY_Msk ) {
        if(comms_state == 1) {
            //Read operation
            
            if(comms_buffer_offset < comms_buffer_size) {
                I2CS_PORT->SERCOM_DATA = comms_buffer[comms_buffer_offset++];
            } else {
                I2CS_PORT->SERCOM_DATA = 0xFF;
            }
            
            I2CS_PORT->SERCOM_CTRLB = I2CS_PORT->SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
            I2CS_PORT->SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(0x03);
        } else {
            
            if(comms_buffer_offset < COMMS_BUFF_SIZE) {
                comms_buffer[comms_buffer_offset++] = I2CS_PORT->SERCOM_DATA;
                comms_buffer_size++;
            }
            
            switch(comms_addr) {
                case 0x00:
                    if(comms_buffer_size == 4) {
                        data_value = (comms_buffer[0] << 24) |
                                (comms_buffer[1] << 16) |
                                (comms_buffer[2] << 8) |
                                (comms_buffer[3] << 0);
                    }
                    
                    break;
                case 0x01:
                    if(comms_buffer_size == 4) {
                        clicks = (comms_buffer[0] << 24) |
                                (comms_buffer[1] << 16) |
                                (comms_buffer[2] << 8) |
                                (comms_buffer[3] << 0);
                    }
                    
                    break;
            }
            
            I2CS_PORT->SERCOM_CTRLB = I2CS_PORT->SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
            I2CS_PORT->SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(0x03);
        }
    } else if(I2CS_PORT->SERCOM_INTFLAG & SERCOM_I2CS_INTENSET_AMATCH_Msk ) {
        comms_addr = (I2CS_PORT->SERCOM_DATA >> 1) - I2C_ADDRESS;
        
        comms_buffer_offset = 0;
        comms_buffer_size = 0;
        
        if(I2CS_PORT->SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) {
            //Peripheral read operation
            comms_state = 1;
            
            switch(comms_addr){
                case 0:
                    comms_buffer[comms_buffer_size++] = data_value >> 24;
                    comms_buffer[comms_buffer_size++] = data_value >> 16;
                    comms_buffer[comms_buffer_size++] = data_value >> 8;
                    comms_buffer[comms_buffer_size++] = data_value >> 0;
                    
                    data_value = 0;
                    
                    break;
                case 1:
                    comms_buffer[comms_buffer_size++] = clicks >> 24;
                    comms_buffer[comms_buffer_size++] = clicks >> 16;
                    comms_buffer[comms_buffer_size++] = clicks >> 8;
                    comms_buffer[comms_buffer_size++] = clicks >> 0;
                    
                    clicks = 0;
                    
                    break;
            }

        } else {
            //Peripheral write operation
            comms_state = 2;
        }
        
        //Clear the match interrupt
        I2CS_PORT->SERCOM_INTFLAG |= SERCOM_I2CS_INTENSET_AMATCH_Msk;
        I2CS_PORT->SERCOM_CTRLB = I2CS_PORT->SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
        I2CS_PORT->SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(0x03);
        
    } else if(I2CS_PORT->SERCOM_INTFLAG & SERCOM_I2CS_INTENSET_ERROR_Msk) {
        comms_state = 0xFF;
    }
}
