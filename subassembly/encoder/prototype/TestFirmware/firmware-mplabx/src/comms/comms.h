/* 
 * File:   comms.h
 * Author: benvh
 *
 * Created on May 29, 2021, 1:47 PM
 */

#ifndef COMMS_H
#define	COMMS_H

#ifdef	__cplusplus
extern "C" {
#endif

    void comms_init();
    
    void comms_update(uint32_t new_clicks, int32_t encoder_data);
    
    void comms_get_command(uint8_t *command, uint8_t *data);



#ifdef	__cplusplus
}
#endif

#endif	/* COMMS_H */

