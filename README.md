# Z80-John-Bell-80-280
Hardware reverse-engineering and software for 1980 Z-80 SBC 80-280 by John Bell Engineering.  
I have a 1980 vintage Z80 single-board computer model 80-280 by John Bell Engineering.
I wanted to try running it, but I needed to know the addresses of the EPROM, the RAM, and the PIO.
I could not find the information on the web (though it later became available),
so I traced the circuit board wiring and prepared a schematic diagram.
I also built a circuit to read the EPROM content.  
This project records the results of my efforts.

## Z-80 Computer 80-280 by John Bell Engineering

The 80-280 Z-80 single-board computer is a 4.15" x 3.3" PC board with a 25 pin edge connector.
The board hosts a Z80 CPU, Z80 PIO, 2Kx8 2716 or 4Kx8 2532 EPROM,
and a pair of 1Kx4 2114 static RAMs that provide 1K bytes of read/write memory.
A 74LS04 Hex Inverter and a 74LS10 Triple 3-input NAND provide a clock generator and address decoding.
A 4.7K&ohm resistor and 10&mu.F electrolytic capacitor provide an power-on auto-reset circuit.
A jumper is provided to optionally route the PIO interrupt request to the Z80 INT or NMI input.
This is pretty close to a minimal Z80 computer.

### Clock Generator

Two of the inverters on the 74LS04 are cross-connected to form an RC-timed clock generator circuit.
A third inverter acts as a buffer to distribute the clock signal to the Z80 CPU and the Z80 PIO.
The clock signal is about 1.8 MHz, measured with an oscilloscope.

### On-board Power-on Auto-Reset

The board I have did not include the capacitor for the power-on auto-reset circuit.
I was left to conjecture what its value might be.
The 10 uF value on the schematic matches the value in the documentation for the board,
which was made available online in an ebay post.
I have not yet tested what delay the combination of 4.7K&ohm. and 10&mu.F provides.
If the Z80 /RESET input threshold matches TTL specifications
(I~l~ <= 0.8 volts, 2.0 volts <= I~h~),
then R=4.7K&ohm., C=10&mu.F, provides a /RESET low signal between 8.2 and 24 milliseconds long from power on.

### External Power-on Auto-Reset

The John Bell Engineering documentation for their 6502 SBC recommends an external power-on auto-reset
circuit based on a 555 Timer IC.  The 555 Timer has precisely specified thresholds at 1/3 and 2/3 Vcc,
so the delay can be precisely calculated for particular RC combinations.
The recommended circuit uses R=1M&ohm., C=0.1&mu.F, for a delay of 110 milliseconds.  
Note that the recommended circuit connects the capacitor between Vcc and pins 2 and 6,
the resistor between pins 2 and 6 and ground.
The input to pins 2 and 6 is a decaying exponential that begins a Vcc and drops to ground.
The circuit takes the /RESET signal from pin 7, the discharge pin, and uses the 4.7K&ohm. on the board
as a pull-up resistor.
This provides a low logic level until the input signal decays to 1/3 Vcc.
The delay is calculated as:
    Delay = -ln(1/3)RC seconds
          = 1.098612(1 &times. 10^6^)(0.1 &times. 10^-6^) seconds
          = 1.098612 &times 10^-1^ seconds
          = 109.8612 &times. 10^-3^ seconds
          = 109.8612 milliseconds  
When using an external power-on auto-reset circuit, the 10&mu.F capacitor is not installed on the board.

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

