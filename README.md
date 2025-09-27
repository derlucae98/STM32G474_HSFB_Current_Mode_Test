# STM32G474_HSFB_Current_Mode_Test
First steps of digitally controlling a hard-switched full-bridge DC/DC converter using a NUCLEO-G474RE

### Test of the 2p2z controller without the power stage:
1) Connect ADC input to DAC output via 10 Ohm resistor
2) Set compensator gain to 1
3) Set reference value to half of ADC resolution
4) Controller will control ADC input to Vref/2
5) Inject VNA signal across 10 Ohm resistor
6) Measure with VNA at ADC input and DAC output

![Plot of simulated and measured tranfer function](https://github.com/derlucae98/STM32G474_HSFB_Current_Mode_Test/blob/7c430e59830eb0678a4b55953c3e111bdf03e4b3/Tests/2p2z_first_test.png)

Magnitude matches the simulation perfectly up to the Nyquist frequency (125 kHz). Phase deviates beyond 3 kHz from simulation. Some tuning of the sampling time within the cycle is required. 
