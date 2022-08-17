# Z80-John-Bell-80-280
Hardware reverse-engineering and software for 1980 Z-80 SBC 80-280 by John Bell Engineering.  
I have a 1980 vintage Z80 single-board computer model 80-280 by John Bell Engineering.
I wanted to try running it, but I needed to know the addresses of the EPROM, the RAM, and the PIO.
I could not find the information on the web (though it later became available),
so I traced the circuit board wiring and prepared a schematic diagram.
I also built a circuit to read the EEPROM content.  
This project records the results of my efforts.

## Z-80 Computer 80-280 by John Bell Engineering

The 80-280 Z-80 single-board computer is a 4.15" x 3.3" PC board with a 25 bin edge connector.
The board hosts a Z80 CPU, Z80 PIO, 2Kx8 2716 or 4Kx8 2532 EPROM,
and a pair of 1Kx4 2114 static RAMs that provide 1Kx8 bytes of read/write memory.
A 74LS04 Hex Inverter and a 74LS10 Triple 3-input NAND provide a clock generator and address decoding.
A 4.7K resistor and 10 microfarad capacitor provide an auto-reset circuit.
A jumper is provided to optionally route the PIO interrupt request to the Z80 INT or NMI input.
This is pretty close to a minimal Z80 computer.

### Clock Generator


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

