
## 2022-Nov-06

Improvement thoughts before next manuf run:

- Identifying different modules
  - Multiple bits of ID using multple UP/PD resistors
  - ADC channel with a resistor divider
    - 10 bit ADC built in to SAMD10 should allow many discrete values, even with tolerances
  - Pins
    - ~~1, 2, 3, 4, 5,~~ 6, ~~9, 10,~~ ~~11, 12~~
      -  9, 10 are used for encoder in.  These could also be 
- DAC
  - Any reason to expose the DAC?
    - Can't drive a useful volume level through the SMD $18\omega$ speakers at 3.3V
  - Only available on Pin 1, currently used for channel A.  
    - Could easily shift channels down one pin without significant rerouting.
- External Interrupt Controller
  - Used for encoder input, could also expose additional pins.
  - Not currently used for switch but it is on a valid pin for it.
- SPI
  - Any reason to add in SPI Support?  
    - Requires more pins but allows higher data rate
    - Chaining difficulties, each module requires it's on ~CS
- Connections
  - Some daisy-chainable connection at board edge to allow easy horizontal rows
    - 


## Pins


### HW 2022-11-06

#### Option 1

Free pin 1 for ADC/DAC

- 1 - 
  - ADC 
- 2 - LED Column A
- 3 - LED Column B
- 4 - LED Column C
- 5 - LED Column D
- 6 - LED Column E
- 9 - ENC B input
  - EIC
- 10 - ENC A input
  - EIC
- 11 - I2C SDA
  - Limited options per DS
- 12 - I2C SCL
  - Limited options per DS
- 13 - INT output
- 17 - SW input
- 19 - SWCLK
  - Limited options per DS
- 20 - SWDIO
  - Limited options per DS

### HW 2021-03-27

- 1 - LED Column A
- 2 - LED Column B
- 3 - LED Column C
- 4 - LED Column D
- 5 - LED Column E
- 9 - ENC B input
  - EIC
- 10 - ENC A input
  - EIC
- 11 - I2C SDA
  - Limited options per DS
- 12 - I2C SCL
  - Limited options per DS
- 13 - INT output
- 17 - SW input
- 19 - SWCLK
  - Limited options per DS
- 20 - SWDIO
  - Limited options per DS