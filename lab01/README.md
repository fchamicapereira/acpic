## Interface's power consumption estimation

### Definitions

Mean power estimation, resistor power and LED power:

![definitions_p](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_p.svg?sanitize=true)

Current:

![definitions_i](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_i.svg?sanitize=true)

Resistor:

![definitions_r](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_r.svg?sanitize=true)

Voltage:

![definitions_v](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_v.svg?sanitize=true)

### Calculations


Getting the voltage drop on each LED:

![calc_vc](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_vc.svg?sanitize=true)

Getting the current from the voltage read on each resistor:

![calc_i](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_i.svg?sanitize=true)

Now the power consumption of the LEDs and resistors:

![calc_p](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_p.svg?sanitize=true)

And finally the mean power estimation of the whole system.
Each second, a different LED is turned on, and all the others are turned off. By the fifth second,
all the LEDs are turned off. As such, each LED is turned on 20% of the time, and 20% of the time
no LED is turned on.

The final power estimation is given by the following expression:

![calc_p_final](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_p_final.svg?sanitize=true)

Simplifying:

![calc_p_final](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_p_final_simplified.svg?sanitize=true&1570706542338)
