Keyboard Matrix Board




## General

- Sub-module for keyboard and keys
- Supports SK6812 LEDs for some number of those keys
  - 5V level shifting circuit in place but bypass jumper available
- Supports one octave of inputs + 1
  - Key matrix for reading inputs
- I2C for communication as peripheral
  - 3.3V logic levels for I2C

## Keys

- 8 Mode Selects
  - 8 would be optimal since it could allow a metronome/sequencer, possibly.
  - Clear
  - LEDs
- 5 black keys
- 8 white keys
  - Started with 7 but 8 allowed visual alignment with mode selects and provides some options
    - e.g. if playing, seeing the user play the high end of the octave could switch octaves up?
- Matrix
  - 5 Row sense lines
  - 5 Column drive lines
    - One drive line is only needed in one place to support next octave C
  - Rows are grouped by area
  - Columns are laid out for ease of routing only

## Mode Selects



## LEDs

- Chained SK6812s
  - LED line weaves through keys so order is not intuitive