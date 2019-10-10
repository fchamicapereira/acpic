## Estimation of the power consumption of the interface

## Definitions

Mean power estimation, resistence power and LED power:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_p.svg)

Current:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_i.svg)

Resistence:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_r.svg)

Voltage:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/definitions_v.svg)

## Calculations


Getting the voltage drop on each LED:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_vc.svg)

From there, we can get the current:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_i.svg)

Now the power consumption of the LEDs and resistences:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_p.svg)

And finally the mean power estimation of the whole system.
Each second, a different LED is turned on, and all the others are turned off. By the fifth second,
all the LEDs are turned off. As such, each LED is turned on 20% of the time, and 20% of the time
no LED is turned on.

As such, the final power estimation is given by the following expression:

![alt text](https://raw.githubusercontent.com/fchamicapereira/acpic/master/lab01/images/calc_p_final.svg)
