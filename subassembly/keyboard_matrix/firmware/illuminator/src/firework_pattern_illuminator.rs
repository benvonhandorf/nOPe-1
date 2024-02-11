pub use crate::illuminator::Illuminator;

use crate::data::*;

use crate::keystrike_animation::*;

use keyboard_matrix::KeyboardState;

use smart_leds::hsv;
use synth_engine::SynthState;

use smart_leds::hsv::hsv2rgb;
use smart_leds::hsv::Hsv;
use smart_leds::hsv::RGB8;

pub struct FireworkPatternIlluminator {
    total_time_ms: u32,
    idle_time_ms: u32,
    keys: [Hsv; 21],
}

impl FireworkPatternIlluminator {
    pub fn new() -> Self {
        Self { 
            total_time_ms: 0, 
            idle_time_ms: 0, 
            keys: [Hsv { hue: 0, sat: 0, val: 0 }; 21]
        }            
    }
}

impl Illuminator for FireworkPatternIlluminator {
    fn update(
        &mut self,
        delta_t_ms: u32,
        keyboard_state: &KeyboardState,
        synth_state: &SynthState,
    ) {
        self.total_time_ms = self.total_time_ms.wrapping_add(delta_t_ms);
        self.idle_time_ms = self.idle_time_ms.wrapping_add(delta_t_ms);

        if self.idle_time_ms > 1000 {
            self.idle_time_ms = 0;

            let key_index = (self.total_time_ms / 7) % 21;
            let hue = self.total_time_ms as u8;
            self.keys[key_index as usize] = Hsv {
                hue: hue,
                sat: 30,
                val: 255,
            };

            adjacency_recursion(
                255,
                key_index as u8,
                0,
                &mut |index, recurse_level| {
                    self.keys[index as usize] = Hsv {
                        hue: hue,
                        sat: 50,
                        val: 127,
                    };
                },
            );
        }

        for hsv in self.keys.iter_mut() {
            if hsv.val > 0 {
                hsv.val -= 1;
            }
            if hsv.sat < 255 {
                hsv.sat += 2;
            }
        }
    }

    fn render(&mut self, leds: &mut [RGB8; 21]) {
        for (index, pixel) in leds.iter_mut().enumerate() {
            let hsv = &self.keys[index];

            *pixel = hsv2rgb(*hsv);
        }
    }
}
