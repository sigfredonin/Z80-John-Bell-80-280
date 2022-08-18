# Z80-John-Bell-80-280
*Hardware reverse-engineering and software for 1980 Z-80 SBC 80-280 by John Bell Engineering.*

I have a 1980 vintage Z80 single-board computer model 80-280 by John Bell Engineering.
I wanted to try running it, but to program it I would need to know the addresses of the EPROM, the RAM, and the PIO.
I could not find the information on the web (though it later became available),
so I traced the circuit board wiring and prepared a schematic diagram.
I also built a circuit to read the EPROM content.

This project records the results of my efforts.

## Z-80 Computer 80-280 by John Bell Engineering

The 80-280 Z-80 single-board computer is a 4.15" x 3.3" PC board with a 25-pin edge connector
on each side (only the component side is used).
The board hosts a Z80 CPU, Z80 PIO, 2Kx8 2716 or 4Kx8 2532 EPROM,
and a pair of 1Kx4 2114 static RAMs that provide 1K bytes of read/write memory.
A 74LS04 Hex Inverter and a 74LS10 Triple 3-input NAND provide a clock generator and address decoding.

The Z80 CPU /RESET and /WAIT inputs are brought out to the edge connector, as are both PIO ports.
A jumper is provided to optionally route the PIO interrupt request to the Z80 CPU /INT or /NMI input.
The board provides 4.7K&ohm; pull-up resistors for the Z80 CPU /RESET, /WAIT, /INT, and /NMI inputs, and
for the unused Z80 CPU /BUSREQ and PIO /IEI.

A 10&mu;F electrolytic capacitor can be installed, which together with the 4.7K&ohm; pull-up resistor,
provides a cheap power-on auto-reset circuit.

This is pretty close to a minimal Z80 computer.

### Clock Generator

Two of the inverters on the 74LS04 are cross-connected to form an RC-timed clock generator circuit.
A third inverter acts as a buffer to distribute the clock signal to the Z80 CPU and the Z80 PIO.
The clock signal is about 1.8 MHz, measured with an oscilloscope.

### On-board Power-on Auto-Reset

The board I have did not include the capacitor for the power-on auto-reset circuit.
I was left to conjecture what its value might be.
The 10&mu;F value on the schematic matches the value in the documentation for the board,
which was made available online in an ebay post.
I have not yet tested what delay the combination of 4.7K&ohm; and 10&mu;F provides.
If the Z80 /RESET input threshold matches TTL specifications
(I<sub>L</sub> <= 0.8 volts, 2.0 volts <= I<sub>H</sub>),
then R=4.7K&ohm;, C=10&mu;F, provides a /RESET low signal between 8.2 and 24 milliseconds long from power on.
To confirm that, I would have to measure the delay from power-on to the first /M1 pulse.

### External Power-on Auto-Reset

The John Bell Engineering documentation recommends an external power-on auto-reset
circuit based on a 555 Timer IC.  The 555 Timer has precisely specified thresholds at 1/3 and 2/3 Vcc,
so the delay can be precisely calculated for particular RC combinations.
The recommended circuit uses R=1M&ohm;, C=0.1&mu;F, for a delay of 110 milliseconds.

Note that the recommended circuit connects the capacitor between Vcc and pins 2 and 6,
and the resistor between pins 2 and 6 and ground.
The input to pins 2 and 6 is a decaying exponential e<sup>T/RC</sup> that begins at Vcc and drops to ground,
because the capacitor has no charge when power is applied.
The circuit takes the /RESET signal from pin 7, the discharge pin, and uses the 4.7K&ohm; on the board
as a pull-up resistor.
This provides a low logic level until the input signal decays to 1/3 Vcc.

The delay is calculated as:

Delay = -ln(1/3)RC seconds  
      = 1.098612(1 &times; 10<sup>6</sup>)(0.1 &times; 10<sup>-6</sup>) seconds  
      = 1.098612 &times; 10<sup>-1</sup> seconds  
      = 109.8612 &times; 10<sup>-3</sup> seconds  
      = 109.8612 milliseconds  

When using an external power-on auto-reset circuit, the 10&mu;F capacitor is not installed on the board.

### How Long Should Power-on /RESET be?

The Z80 CPU documentation shows an example reset circuit that includes a power-on auto-reset feature,
implemented with an R=10K&ohm;, C=68&mu;F rising exponential delay 1-e<sup>T/RC</sup>,
and a 74132 schmitt trigger gate with a nominal rising voltage threshold of 1.7 volts.
That circuit should provide a delay of about 1/3 second:

Delay = -ln(1-(1.7/5))RC seconds  
      = 0.415515(10 &times; 10<sup>3</sup>)(68 &times; 10<sup>-6</sup>) seconds  
      = 2.82551 &times; 10<sup>-1</sup> seconds  
      = 282.551 &times; 10<sup>-3</sup> seconds  
      = 282.551 milliseconds  

The on-board power-on auto-reset circuit would provide between 8.2 and 24 milliseconds
(if my calculations are correct; a test with a 7404 inverter measured ~8 milliseconds).

The recommended external 555 Timer circuit provides a 110 millisecond delay.

What's appropriate?

How is one to know?

In his book *Build Your Z80 Own Computer*, Steve Ciarcia gives as the reason for
an automatic power-on reset as causing the CPU to delay executing instructions
until the rest of the computer is powered up.
He mentions delays of 35-50 milliseconds for a computer that will eventually
include memory and I/O subsystems, including parallel and serial I/O,
a keyboard, and a CRT controller for an ASCII terminal.
However, for a single-board computer, such as the John Bell Engineering 80-280,
the designer has no way to know how long it will take the remainder of the
system to be ready to take instructions from the computer.
The components on the board will likely be ready to go in a few tens of microseconds.
The rest of the system, who knows?

It is up to the board user, and since for the 80-280 the interface to the rest of
the system is the PIO ports, the user can choose to program a delay or a handshake
to verify system readiness.

## TEST 1 EPROM

The board I have came with a 2716 EPROM labelled TEST 1.
I was not sure that a 2716 programmed around 1980 would still be readable,
but in fact it contains only a few bits in error far from the test program,
which occupies the first 18 bytes.
The test program outputs a two-byte count, 0000..FFFF, to the two ports on the PIO.

### EPROM Reader

The EPROM reader, which I built with 3 shift register ICs on a solderless breadboard
and a short C program on an Arduino compatible micro-controller, read the content of
the 2716 EPROM and wrote it in hexadecimal to the serial terminal emulator on the attached PC.
From there, I could copy and paste it into a text file.

This was sufficient for my purposes, since I could convert the hex text to binary with a
separate program.

### Decompiling

