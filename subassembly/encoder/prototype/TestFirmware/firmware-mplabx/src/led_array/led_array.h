/* 
 * File:   led_array.h
 * Author: benvh
 *
 * Created on April 10, 2021, 6:37 PM
 */

#ifndef LED_ARRAY_H
#define	LED_ARRAY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define LED_ARRAY_COUNT 20

    void led_array_init();
    void led_array_tick();
    void led_array_set_led(uint8_t position, uint8_t value);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_ARRAY_H */

