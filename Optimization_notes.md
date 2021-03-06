# Optimization

## Setup

I'm starting the optimization from the first commit.

I wish I had a spare IO pin to toggle for a reliable loop indicator. Instead I'm putting a scope probe on the cathode of D2. That gets toggled once per loop.

The timing jumps all over the place, so I disabled a few things.
    Disabled the check for the trigger rising edge. It always does the quantization and updates the LED.
    Disabled the check for if the current pitch is different from the target. It always writes to the DAC.

There's still a fair bit of timing jitter based on the input CV. I hooked it up to the BB8, which is stopped. With the BB8 knob at minimum, +/-1V switch at 0V, and the quantizer's shift pot at minimum, the period is 1.81 ms. With the BB8 knob at max, +1V, and the shift knob at max, the period is 3.02 ms. Perhaps this isn't the maximum quantization time, but I'll take it as such for these figures.

Actually, I'm switching to the signal generator. With it producing 10V DC (acutally 2mV sine wave with 9.999V offset), I can tell that the ADC is maxed out. That produces a maximum delay of 2.98 ms.

There's still a little bit of jitter, about 10 us.

## Changes

### digitalWriteFast

Port IO is known to be slow on Arduino. I switched to digitalWriteFast() and saved about 60 us. I also tried pinModeFast(), but that gave some odd results and only saved 30 us.

### digitalRead port access

digitalRead is also slow. I switched to reading the port directly. This saved 120 us. I don't know why the 10V in isn't showing any time saved.

I needed to add a 2 us delay after setting the column pin before checking the row pin. Otherwise there wasn't enough time for the signal to propagate. Now that's fast!

### Quantization optimization

I tried replacing the multiplication with bitshifts, but it didn't make any difference. I think the compiler is already doing that.

Then I tried adding a check at the outer loop to see if it was even in the right octave. This makes sure the inner loop only runs once. Now the quantization doesn't really take any longer for higher octaves. The quantization time varies by about 200 us.

Then I changed the note variable to be 8 bits instead of 16. This cut the quantization time in about half.

### SPI

The SPI peripheral is faster than shiftOut(). Since I'm already using the SS pin for one of the columns, I need to use SPI.begin() and SPI.end() before and after each transfer.

## Results

| Change            | 0V in | 10V in |
| ----------------- | ----- | ------ |
| Baseline          |  1.81 |   2.98 |
| digitalWriteFast  |  1.75 |   2.86 |
| digitalRead port  |  1.62 |   2.86 |
| Quantization      |  1.63 |   1.73 |
| SPI               |  1.45 |   1.57 |