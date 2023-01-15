
#include "startup_gcc.h"

#include <stdbool.h>
#include <stddef.h>

#include "device.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint32_t _svectors;

extern int main(void);
extern void __attribute__((long_call)) __libc_init_array(void);

/* Device Vector information is available in interrupt.c file */

extern void Dummy_Handler(void);

/* Brief default application function used as a weak reference */
void __attribute__((optimize("-O1"), long_call)) Dummy_Handler(void) { return; }

/* Optional application-provided functions */
extern void __attribute__((weak, long_call, alias("Dummy_Handler")))
_on_reset(void);
extern void __attribute__((weak, long_call, alias("Dummy_Handler")))
_on_bootstrap(void);

/* Device vectors list dummy definition*/
extern void SVCall_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void PendSV_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void SysTick_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void PM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void SYSCTRL_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void WDT_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void RTC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void NVMCTRL_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void DMAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void EVSYS_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void SERCOM1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void SERCOM2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void TCC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void TC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void TC2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void ADC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void AC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void DAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
extern void PTC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

/* Multiple handlers for vector */
__attribute__((section(".vectors"))) const DeviceVectors exception_table = {
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .pvStack = &_estack,

    .pfnReset_Handler = Reset_Handler,
    .pfnNonMaskableInt_Handler = NonMaskableInt_Handler,
    .pfnHardFault_Handler = HardFault_Handler,
    .pfnSVCall_Handler = SVCall_Handler,
    .pfnPendSV_Handler = PendSV_Handler,
    .pfnSysTick_Handler = SysTick_Handler,
    .pfnPM_Handler = PM_Handler,
    .pfnSYSCTRL_Handler = SYSCTRL_Handler,
    .pfnWDT_Handler = WDT_Handler,
    .pfnRTC_Handler = RTC_Handler,
    .pfnEIC_Handler = EIC_Handler,
    .pfnNVMCTRL_Handler = NVMCTRL_Handler,
    .pfnDMAC_Handler = DMAC_Handler,
    .pfnEVSYS_Handler = EVSYS_Handler,
    .pfnSERCOM0_Handler = SERCOM0_Handler,
    .pfnSERCOM1_Handler = SERCOM1_Handler,
    .pfnSERCOM2_Handler = SERCOM2_Handler,
    .pfnTCC0_Handler = TCC0_Handler,
    .pfnTC1_Handler = TC1_Handler,
    .pfnTC2_Handler = TC2_Handler,
    .pfnADC_Handler = ADC_Handler,
    .pfnAC_Handler = AC_Handler,
    .pfnDAC_Handler = DAC_Handler,
    .pfnPTC_Handler = PTC_Handler,
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void __attribute__((optimize("-O1"), section(".text.Reset_Handler"), long_call,
                    noreturn)) Reset_Handler(void) {
    uint32_t *pSrc, *pDest;

    /* Initialize the relocate segment */
    pSrc = &_etext;
    pDest = &_srelocate;

    if (pSrc != pDest) {
        for (; pDest < &_erelocate;) {
            *pDest++ = *pSrc++;
        }
    }

    /* Clear the zero segment */
    for (pDest = &_szero; pDest < &_ezero;) {
        *pDest++ = 0;
    }

    /* Call the optional application-provided _on_reset() function. */
    _on_reset();

    /* Initialize the C library */
    __libc_init_array();

    /* Call the optional application-provided _on_bootstrap() function. */
    _on_bootstrap();

    /* Branch to application's main function */
    int retval = main();
    (void)retval;

    /* Infinite loop */
    while (true) {
    }
}
