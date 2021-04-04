I'm starting the optimization from the first commit.

I wish I had a spare IO pin to toggle for a reliable loop indicator. Instead I'm putting a scope probe on the cathode of D2. That gets toggled once per loop.

The timing jumps all over the place, so I disabled a few things.
    Disabled the check for the trigger rising edge. It always does the quantization and updates the LED.
    Disabled the check for if the current pitch is different from the target. It always writes to the DAC.

There's still a fair bit of timing jitter based on the input CV. I hooked it up to the BB8, which is stopped. With the BB8 knob at minimum, +/-1V switch at 0V, and the quantizer's shift pot at minimum, the period is 1.81 ms. With the BB8 knob at max, +1V, and the shift knob at max, the period is 3.02 ms. Perhaps this isn't the maximum quantization time, but I'll take it as such for these figures.

Actually, I'm switching to the signal generator. With it producing 10V DC (acutally 2mV sine wave with 9.999V offset), I can tell that the ADC is maxed out. That produces a maximum delay of 3.98 ms.

There's still a little bit of jitter, about 10 us.


Results

| Change        | 0V in | 10V in |
| ------------- | ----- | ------ |
| Baseline      |  1.81 |   3.98 |
| 