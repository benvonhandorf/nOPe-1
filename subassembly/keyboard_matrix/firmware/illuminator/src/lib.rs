#![no_std]

mod illuminator;
mod data;
mod keystrike_illuminator;
mod keystrike_animation;
mod rainbow_pattern_illuminator;
mod firework_pattern_illuminator;

use illuminator::Illuminator;
use keystrike_illuminator::KeystrikeIlluminator;

use rainbow_pattern_illuminator::RainbowPatternIlluminator;
use firework_pattern_illuminator::FireworkPatternIlluminator;

use keyboard_matrix::KeyboardState;
use synth_engine::SynthState;

use smart_leds::{hsv::RGB8, SmartLedsWrite};

#[derive(PartialEq)]
enum IlluminationMode {
    Keystrike,
    RainbowPattern,
    FireworkPattern,
}

pub struct IlluminationEngine<'a, StrandType> {
    led_strand: &'a mut StrandType,
    led_data: [RGB8; 21],
    keystrike_illuminator: KeystrikeIlluminator,
    rainbow_pattern_illuminator: RainbowPatternIlluminator,
    firework_pattern_illuminator: FireworkPatternIlluminator,
    illumination_mode: IlluminationMode,
    idle_time_ms: u32,
    total_time_ms: u32,
}

const IDLE_MODE_TIMEOUT_MS: u32 = 5000;

impl<'a, LedStrand> IlluminationEngine<'a, LedStrand>
where
    LedStrand: SmartLedsWrite<Error = (), Color = RGB8>,
{
    pub fn new(led_strand: &'a mut LedStrand) -> Self {
        Self {
            led_strand: led_strand,
            led_data: [RGB8::default(); 21],
            keystrike_illuminator: KeystrikeIlluminator::new(),
            rainbow_pattern_illuminator: RainbowPatternIlluminator::new(),
            firework_pattern_illuminator: FireworkPatternIlluminator::new(),
            illumination_mode: IlluminationMode::Keystrike,
            idle_time_ms: 0,
            total_time_ms: 0,
        }
    }

    pub fn update(&mut self, delta_t_ms: u32, keyboard_state: &KeyboardState, synth_state: &SynthState) {
        self.total_time_ms = self.total_time_ms.wrapping_add(delta_t_ms);

        if keyboard_state.depressed_count == 0 {
            self.idle_time_ms = self.idle_time_ms.saturating_add(delta_t_ms);
        } else {
            self.idle_time_ms = 0;
        }

        self.keystrike_illuminator.update(delta_t_ms, keyboard_state, synth_state);

        if self.idle_time_ms < IDLE_MODE_TIMEOUT_MS {
            self.illumination_mode = IlluminationMode::Keystrike;
        } else if self.illumination_mode == IlluminationMode::Keystrike {
            //Transitioning out of Keystrike, pick an idle mode at "random"
            if (self.total_time_ms / 8 ) % 2 == 0 {
                self.illumination_mode = IlluminationMode::FireworkPattern;
            } else {
                self.illumination_mode = IlluminationMode::RainbowPattern;
            }
        }

        match self.illumination_mode {
            IlluminationMode::RainbowPattern => {
                self.rainbow_pattern_illuminator.update(delta_t_ms, keyboard_state, synth_state);
            }
            IlluminationMode::FireworkPattern => {
                self.firework_pattern_illuminator.update(delta_t_ms, keyboard_state, synth_state);
            }      
            _ => {}      
        }
    }

    pub fn render(&mut self) {

        for i in 0..21 {
            self.led_data[i] = RGB8::default();
        }

        match self.illumination_mode {
            IlluminationMode::Keystrike => {
                self.keystrike_illuminator.render(&mut self.led_data);
            }
            IlluminationMode::RainbowPattern => {
                self.rainbow_pattern_illuminator.render(&mut self.led_data);
            }
            IlluminationMode::FireworkPattern => {
                self.firework_pattern_illuminator.render(&mut self.led_data);
            }            
        }

        self.led_strand
            .write(self.led_data.iter().cloned())
            .unwrap();
    }
}
