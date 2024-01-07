## Overview

This code is the Rust firmware for the Keyboard Input Board of the nOPE-1 project.

## Layout

`firmware` is the root of the main firmware.  It includes `main` along with the BSP definition for the hardware.  The communication protocol is also implemented here.

`comms` abstracts out the I2C state machine.

`keyboard_matrix` handles keyboard matrix polling.

`synth_engine` incorporates logic interpreting the keyboard state and maintaining the state of the "synth", including which keys are being played, which octave is selected, etc.

`illuminator` contains all the logic for driving the LED array, including adjacency, reacting to key presses, fading, etc.