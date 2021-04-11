/* 
 * File:   encoder.h
 * Author: benvh
 *
 * Created on April 10, 2021, 6:31 PM
 */

#ifndef ENCODER_H
#define	ENCODER_H

#ifdef	__cplusplus
extern "C" {
#endif

    void encoder_init();
    
    void encoder_interrupt();
    
    void encoder_tick();
    
    int32_t encoder_get_increment();
    
    uint8_t encoder_get_clicks();



#ifdef	__cplusplus
}
#endif

#endif	/* ENCODER_H */

