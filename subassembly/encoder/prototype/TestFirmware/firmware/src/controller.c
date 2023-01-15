
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>

#include "controller.h"
#include "led_array/led_array.h"
#include "comms/comms.h"
#include "encoder/encoder.h"

#define ANIMATION_TYPES 3

static uint8_t mode = 0;
static int8_t led_focused = 0;
static int8_t type = 0;
static int16_t speed = 1;
static uint32_t counter = 0;
static const uint8_t tail = 2;

void clear_leds() {
    for (uint8_t led_counter = 0; led_counter < LED_ARRAY_COUNT; led_counter++) {
        led_array_set_led(led_counter, 0);
    }
}

void process_init() {
    clear_leds();

    led_focused = 0;

    led_array_set_led(led_focused, 0xFF);
}

void process_switch_mode() {
    mode = !mode;

    clear_leds();
}

//Simple chasing lights with a tail of lower illuminated LEDs behind the focused LED.
void process_type_0(int32_t encoder_data) {

    if (encoder_data) {
        speed += encoder_data;
    }

    clear_leds();

    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;

    led_focused += direction;

    while (led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while (led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }

    for (int8_t i = 0; i <= tail; i++) {
        int8_t led = led_focused + (i * direction);

        while (led < 0) {
            led += LED_ARRAY_COUNT;
        }

        while (led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }

        if (i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }
}

void process_type_1(int32_t encoder_data) {
    if (encoder_data) {
        speed += encoder_data;
    }

    clear_leds();

    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;

    led_focused += direction;

    while (led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while (led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }

    for (uint8_t i = 0; i < tail; i++) {
        int8_t led = led_focused + (i * direction);

        while (led < 0) {
            led += LED_ARRAY_COUNT;
        }

        while (led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }

        if (i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }

    for (uint8_t i = 0; i < tail; i++) {
        int8_t led = led_focused - (i * direction);

        while (led < 0) {
            led += LED_ARRAY_COUNT;
        }

        while (led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }

        if (i == 0) {
            led_array_set_led((uint8_t) led, 255);
        } else {
            led_array_set_led((uint8_t) led, 127);
        }
    }
}

int8_t led_sweep = 0;

void process_type_2(int32_t encoder_data) {
    if (encoder_data) {
        speed += encoder_data;
    }

    clear_leds();

    int8_t direction = speed > 0 ? 1 : speed < 0 ? -1 : 0;

    led_sweep++;

    if (led_sweep >= (2 * LED_ARRAY_COUNT)
            || led_sweep <= -(2 * LED_ARRAY_COUNT)) {
        //After sweeping positively and then negatively, clear
        led_focused += direction;
        led_sweep = 0;
    }

    while (led_focused < 0) {
        led_focused += LED_ARRAY_COUNT;
    }

    while (led_focused >= LED_ARRAY_COUNT) {
        led_focused -= LED_ARRAY_COUNT;
    }

    for (uint8_t i = 0; i < led_sweep; i++) {
        int8_t led = led_focused + (i * direction);

        while (led < 0) {
            led += LED_ARRAY_COUNT;
        }

        while (led >= LED_ARRAY_COUNT) {
            led -= LED_ARRAY_COUNT;
        }

        if (i < LED_ARRAY_COUNT) {
            led_array_set_led((uint8_t) led, 0x20);
        } else {
            led_array_set_led((uint8_t) led, 0);
        }
    }

    led_array_set_led((uint8_t) led_focused, 0xFF);
}

void process_normal(int32_t encoder_data) {
    counter++;

    int8_t abs_speed = speed == 0 ? 1 : speed > 0 ? speed : -speed;

    if ((counter * abs_speed) >= 100000) {
        counter = 0;
        switch (type) {
            case 0:
                process_type_0(encoder_data);
                break;
            case 1:
                process_type_1(encoder_data);
                break;
            case 2:
                process_type_2(encoder_data);
                break;
        }
    } else if (encoder_data) {
        speed += encoder_data;
    }
}

void process_mode_adjustment(int32_t encoder_data) {
    if (encoder_data) {
        type += encoder_data;

        while (type < 0) {
            type += ANIMATION_TYPES;
        }
        while (type >= ANIMATION_TYPES) {
            type -= ANIMATION_TYPES;
        }
    }

    for (int i = 0; i < LED_ARRAY_COUNT; i++) {
        led_array_set_led(i, 0);
    }

    led_array_set_led(type, 0xFF);
}

void process_mode(int32_t encoder_data) {
    switch (mode) {
        case 0:
            process_normal(encoder_data);

            break;
        case 1:
            process_mode_adjustment(encoder_data);

            break;
    }
}

void controller_init() {
    led_array_init();

    //EIC is configured by the encoder
    encoder_init();

    process_init();

    comms_init();
}

static uint8_t command;
static uint8_t command_buffer[20];

void controller_tick() {
    led_array_tick();

    encoder_tick();

    uint8_t clicks = encoder_get_clicks();
    int32_t encoder_data = encoder_get_increment();

    comms_update(clicks, encoder_data);

    if (clicks) {
        process_switch_mode();
    }

    process_mode(encoder_data);

    comms_get_command(&command, command_buffer);

    switch (command) {
        case 0x00:
        {
            int32_t command_value = (command_buffer[0] << 24) |
                    (command_buffer[1] << 16) |
                    (command_buffer[2] << 8) |
                    (command_buffer[3] << 0);

            process_mode(command_value);

            break;
        }
        case 0x01:
            process_switch_mode();

            break;
    }
}
