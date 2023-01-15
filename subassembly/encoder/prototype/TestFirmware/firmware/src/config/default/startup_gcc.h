#ifndef __STARTUP_GCC_H__
#define __STARTUP_GCC_H__

#include <stdint.h>

void Reset_Handler (void);
void NonMaskableInt_Handler (void);
void HardFault_Handler (void);
void EIC_Handler (void);
void SERCOM0_Handler (void);

#endif