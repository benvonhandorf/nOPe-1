pub use crate::illuminator::Illuminator;

use crate::data::*;

use crate::keystrike_animation::*;

use keyboard_matrix::KeyboardState;

use synth_engine::SynthState;

use smart_leds::hsv::hsv2rgb;
use smart_leds::hsv::Hsv;
use smart_leds::hsv::RGB8;

use rtt_target::rprintln;

pub struct RainbowPatternIlluminator {
    time_ms: u32,
}

impl RainbowPatternIlluminator {
    pub fn new() -> Self {
        Self { time_ms: 0 }
    }
}

impl Illuminator for RainbowPatternIlluminator {
    fn update(
        &mut self,
        delta_t_ms: u32,
        keyboard_state: &KeyboardState,
        synth_state: &SynthState,
    ) {
        self.time_ms = self.time_ms.wrapping_add(delta_t_ms);
    }

    fn render(&mut self, leds: &mut [RGB8; 21]) {
        let pattern_offset = (self.time_ms % 5000) / 20;
        for (index, pixel) in leds.iter_mut().enumerate() {
            let var = (((index * 13) as u32 + pattern_offset) % 255) as u8;
            let hsv = Hsv {
                hue: var,
                sat: 200,
                val: 50,
            };

            *pixel = hsv2rgb(hsv);
        }
    }
}
