# Z80-John-Bell-80-280
*Hardware reverse-engineering and software for 1980 Z-80 SBC 80-280 by John Bell Engineering.*

| [80-280 component side &rightarrow;](/image/Z80_SBC_Component_Side_Full_Size.JPG) | [80-280 solder side &rightarrow;](/image/Z80_SBC_Component_Solder_Full_Size.JPG) |
| -------------------------------------------------------------------- | ------------------------------------------------------------------- |
| ![80-280 Z80 Computer John Bell Engineering 1980 - component side](/image/Z80_SBC_Component_Side-cropped.jpg "Z80 SBC Component Side") | ![80-280 Z80 Computer John Bell Engineering 1980 - solder side](/image/Z80_SBC_Solder_Side-cropped.jpg "Z80 SBC Solder Side") |

I have a 1980 vintage Z80 single-board computer model 80-280 by John Bell Engineering.
I wanted to try running it, but to program it I would need to know the addresses of the EPROM, the RAM, and the PIO.
I could not find the information on the web (though it later became available),
so I traced the circuit board wiring and prepared a schematic diagram.
I also built a circuit to read the EPROM content.

This project records the results of my efforts.

## Z-80 Computer 80-280 by John Bell Engineering

The 80-280 Z-80 single-board computer is a 4.15" x 3.3" PC board with a 25-pin edge connector
on each side (only the edge connector on the component side is used).
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
| [80-280 Z80 Computer John Bell Engineering 1980 - schematic &rightarrow;](/image/Z80_SBC_80-280_schematic.pdf) |
|----------------------------------------------------------- |
![80-280 Z80 Computer John Bell Engineering 1980 - schematic](/image/Z80_SBC_80-280_schematic_big.PNG "Z80 SBC Schematic")

|       | Address range                                                          |
| ----- | ---------------------------------------------------------------------- |
| EPROM | 0000-07FF (2516, 2716: 2Kx8)                                                 |
|       | 0000-0FFF (2532: 4Kx8)                                                 |
| RAM   | FC00-FFFF (2114 x 2: 1Kx8)                                             |
| PIO   | 00: Port A Data; 01 Port B Data; 02: Port A Control; 03 Port B Control |

|              | Jumper connections                |
| ------------ | --------------------------------- |
| C-16         | 2516 or 2716 EPROM                |
| 32-C         | 2532 EPROM                        |
| /INTREQ-/INT | PIO /INTREQ &rightarrow; CPU /INT |
| /INTREQ-/NMI | PIO /INTREQ &rightarrow; CPU /NMI |

### Clock Generator

Two of the inverters on the 74LS04 are cross-connected to form an RC-timed clock generator circuit.
A third inverter acts as a buffer to distribute the clock signal to the Z80 CPU and the Z80 PIO.
The clock signal is about 1.8 MHz, measured with an oscilloscope.

![80-280 clock generator](/image/Z80_SBC_clock_generator_BW.jpg "Z80 SBC Clock Generator") ![80-280 clock trace](/image/Z80_SBC_Clock_Trace.PNG "Z80 SBC Clock Trace")

### Address Decoding

Address decoding for the EPROM and RAM is provided by random logic using gates in
the 74LS04 Inverter and the 74LS10 Triple 3-Input NAND chips.
Address decoding for the Z80 PIO is done on-chip.

#### PIO Address Decoding

The Z80 PIO uses 4 I/O addresses selected by inputs

- pin 4: /CE Chip Enable (Low signal selects the chip)
- pin 5: C/D Data / Control (Low signal selects data)
- pin 6: B/A Port select (Low signal selects Port A)

The 80-280 board routes /IORQ to the PIO /CE input, since it is the only I/O device.
It routes A0 to the PIO port select input, and A1 to the Data/Control select input.
No external logic is needed.

#### RAM Address Decoding

The RAM is addressed if A15 = 1, and /MREQ is active.

![RAM Address Decoding](/image/Z80_SBC_Address_Decoding_RAM_annotated.png "RAM Address Selection - schematic")

#### EPROM Address Decoding

The EPROM is addressed if A15 = 0, and both /RD and /MREQ are active.

![EPROM Address Decoding](/image/Z80_SBC_Address_Decoding_EPROM_annotated-small.PNG "EPROM Address Selection - schematic")

#### EPROM Type Selection

The 80-280 board includes a jumper to select between a 2Kx8 (16K bit) EPROM and a 4Kx8 (32K bit) EPROM.
The 2Kx8 EPROM can be a 2516 or 2716 EPROM.
The 4Kx8 EPROM can be a 2532 EPROM.
Access time should be 500 nsec or better.

![80-280 EPROM Select](/image/Z80_SBC_Component_Side-2532-2716_jumper-small.PNG "Z80 SBC EPROM Select Jumper")

The 2516 and 2716 EPROMs are pin compatible in read mode in that 
a low signal on pins 18 and 20 reads the byte at the selected address and presents it on the data pins,
and a high signal puts the chip in a low-power state and sets the data pins in a high-impedance state.
The 2532 EPROM requires address bit A11 on pin 18.
A low signal on pin 20 reads the byte at the selected address and presents it on the data pins,
and a high signal puts the chip in a low-power state and sets the data pins in a high-impedance state.

![EPROM 2716 pinout](/image/2716_pinout_annotated-tiny.PNG "EPROM 2716 pinout") ![EPROM 2732 pinout](/image/2732_pinout_annotated-tiny.PNG "EPROM 2732 pinout")

![EPROM 2516 pinout](/image/2516_pinout_annotated-tiny.PNG "EPROM 2516 pinout") ![EPROM 2532 pinout](/image/2532_pinout_annotated-tiny.PNG "EPROM 2532 pinout")

When the EPROM Selection jumper connects 16-C, it routes the EPROM select signal (=&nbsp;/[/A15&nbsp;&&nbsp;RD&nbsp;&&nbsp;MREQ]) to both
pins 18 and 20, appropriate for a 2516 or a 2716 EPROM.
When the EPROM Selection jumper connects C-32, it routes the EPROM select signal to pin 20, and A11 to pin 18,
appropriate for a 2532 EPROM.

Note that this will not work for a 2732 EPROM, which requires A11 on pin 21,
where the other chips have Vpp and require Vcc.
A 2732 has /G (Output Enable, equivalent to /OE) on pin 20
and /E (Chip Enable, equivalent to /CE) on pin 18.
Like the 2716, it requires low signal on pins 18 and 20 to read and present a data byte,
but instead of a high level on pin 21, it requires A11.

#### Address Aliasing

Note that the board logic enables addressing the correct devices
for the *defined address ranges* on the board.

The RAM is selected if A15=1.
A9..A0 select a byte within the RAM;
the values of A14..A10 are ignored.

Similarly, the EPROM is selected if A15=0.
When a 2K EPROM is used, A10..A0 select a byte within the EPROM;
the values of A14..A11 are ignored.
When a 4K EPROM is used, A11..A0 select a byte within the EPROM;
the values of A14..A12 are ignored.

Again similarly, the PIO is selected whenever /IORQ is active.
A1..A0 select the PIO data or control port operated on;
the values of A7..A0 are ignored 
(A15..A9 are not used for I/O addressing - the I/O instructions
cannot specify them).

Because the board logic ignores address bits not necessary to
address the defined ranges, from the program's viewpoint the
address ranges are aliased: the program can specify any value
for the ignored address bits.
It can, for example, address PIO port A control at address F2
instead of 02.
Ordinarily the aliasing does not matter.
No reasonable programmer (by definition!) will use them.
But being aware of the aliasing could help
in understanding the behavior of a malfunctioning program.

A simple example of a program that accesses aliased memory is
one that tries to use more stack space than is available.
When stack pointer drops below the beginning of the RAM space,
it will wrap to the top of RAM and overwrite the data there,
probably the bottom of the stack.
The program will not malfunction until it pops the corrupted
stack data, perhaps much later.
An easy way for this to happen is for a recursive subroutine
to recurse too far.

#### About A15 ...

The Z80 output pins are rated to drive one TTL load.
Most of the Z80 CPU outputs drive the inputs of
the Z80 PIO, the EPROM, and the two 2114 RAM chips,
all of which are rated at 10&mu;A << 1 TTL load.
/MREQ and /RD also drive one input to the EPROM address decode logic.
These are all easily within the capability of the Z80 outputs.

A15, though, is connected to 3:
one in the EPROM address decode logic, and
two in the RAM address decode logic.
This could have been avoided by inverting /A15 from the
EPROM address decode logic using the unused NAND gate U6A.
This would cost one gate more gate delay,
but that's unlikely to cause any problems or slow down memory access.

However ...

The use of Low Power Schottky TTL devices eliminates this potential problem.
Whereas a standard TTL input requires the driver to sink 1.6 mA for a low logic level,
Low Power Schottky TTL only requires 0.4 mA.
The Z80 outputs can sink 1.8 mA when at a low logic level,
sufficient for one standard TTL input, but enough for 4 Low Power Schottky TTL inputs.

### Interrupt Request

The Z80 PIO can request an interrupt on completion of an I/O operation by activating its /INTREQ line (pin 23).
The 80-280 board has provision to optionally route the /INTREQ signal
to the Z80 CPU's /NMI line (non-maskable interrupt, pin 17)
or to the Z80 CPU's /INT line (ordinary maskable interrupt, pin 16).
The through-hole solder lands are 0.1" apart, so pins can be soldered into them
and a jumper used for the connection, or a jumper wire can be soldered across two of them.

![80-280 Z80 Computer John Bell Engineering 1980 - /INTREQ](/image/Z80_SBC_Component_Side-INTREQ_routing-small.PNG "Z80 SBC /INTREQ Routing")

### CPU Reset

The 80-280 board includes provisions to signal the CPU to reset:
it provides an optional on-board power-on auto-reset, and
it routes the CPU /RESET signal to the card edge connector
so that the user can implement off-board reset circuitry.

#### On-board Power-on Auto-Reset

The board I have did not include the capacitor for the power-on auto-reset circuit.
I was left to conjecture what its value might be.
The 10&mu;F value I put on the schematic, based on documentation for John Bell Engineering 80-153 6502 boards,
matches the value in the documentation for the 80-280 board,
which was made available online recently in an ebay post.


![80-280 on-board power-on auto-reset](/image/Z80_SBC_on-board_auto-reset_BW.png "Z80 SBC On-board Power-on Auto-Reset") ![TTL response to RC input](/image/7404_Inverter_Response_to_RC_Input-trimmed-small.PNG "TTL response to RC input - scope trace")  ![CMOS response to RC input](/image/CD4049AE_CMOS_Inverter_Buffer_Response_to_RC_Input-trimmed-small.PNG "CMOS response to RC input - scope trace")

I have not yet tested what delay the combination of 4.7K&ohm; and 10&mu;F provides.
The RC circuit applies a rising exponential 1&#x2011;e<sup>&#x2011;t/RC</sup>
to the Z80 /RESET input.
If the Z80 /RESET input threshold matches TTL specifications
(V<sub>IL</sub> &leq; 0.8 volts, 2.0 volts &leq; V<sub>IH</sub>),
then R=4.7K&ohm;, C=10&mu;F, provides a /RESET low signal between 8.2 and 24 milliseconds long from power on,
without considering typical resistor (5%) and electrolytic capacitor (20%) tolerances.
If the Z80 responds like a CMOS gate, the delay might be very different.

The delays, assuming TTL responses, are calculated as:

Delay = -ln(1-(0.8/5))RC seconds  
      = 0.174353(4.7 &times; 10<sup>3</sup>)(10 &times; 10<sup>-6</sup>) seconds  
      = 8.195 &times; 10<sup>-3</sup> seconds  
      = 8.195 milliseconds  

Delay = -ln(1-(2/5))RC seconds  
      = 0.510826(4.7 &times; 10<sup>3</sup>)(10 &times; 10<sup>-6</sup>) seconds  
      = 2.40088 &times; 10<sup>-2</sup> seconds  
      = 24.0088 &times; 10<sup>-3</sup> seconds  
      = 24.0088 milliseconds  

To confirm the estimate, I would have to measure the delay from power-on to the first /M1 pulse,
using measured resistor and capacitor values.

#### External Power-on Auto-Reset

The John Bell Engineering documentation recommends an external power-on auto-reset
circuit based on a 555 Timer IC.  The 555 Timer has precisely specified thresholds at 1/3 and 2/3 Vcc,
so the delay can be precisely calculated for particular RC combinations.
The recommended circuit uses R=1M&ohm;, C=0.1&mu;F, for a delay of 110 milliseconds.

![80-153 6502 Micro John Bell Engineering 1980 - better auto-reset](/image/80-153_better_auto-reset.PNG "6502 SBC better auto-reset")

Note that the recommended circuit connects the capacitor between Vcc and pins 2 and 6,
and the resistor between pins 2 and 6 and ground.
The input to pins 2 and 6 is a decaying exponential e<sup>&#x2011;t/RC</sup> that begins at Vcc and drops to ground,
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

The uncertainty in this calculation comes from the resistor and capacitor tolerances,
added to a 1% accuracy in the 555 timing and a &plusmn;4% variance in the 555 threshold.
Because the rise time of the 555 output is only ~100 nsec << reset signal time,
the uncertainty in the delay is not much affected by the response of the Z80 /RESET input.

When using an external power-on auto-reset circuit, the 10&mu;F capacitor is not installed on the board.

#### Manual Reset

The Z80 CPU documentation shows an example reset circuit that includes
a manual reset, an external reset, and a power-on auto-reset.

![Z80 reset circuit recommended in Z80 datasheet](/image/Recommended_Z80_Reset_Circuit_from_CPU_datasheet.PNG "Z80 datasheet auto-reset")

The manual and external resets are synchronized to the CPU M1 signal
and converted to a pulse with a one-shot circuit,
in order to avoid losing the content of dynamic memory.
This is not needed for static RAM.

The auto-reset uses a 10K&ohm; resistor and a 68&mu;F capacitor to
generate a rising exponential 1&#x2011;e<sup>&#x2011;t/RC</sup>
as input to a 74132 schmitt trigger gate with a nominal rising voltage threshold of 1.7 volts.

The auto-reset circuit should provide a delay of about 1/4 to 1/3 second from power on:

Delay = -ln(1-(1.7/5))RC seconds  
      = 0.415515(10 &times; 10<sup>3</sup>)(68 &times; 10<sup>-6</sup>) seconds  
      = 2.82551 &times; 10<sup>-1</sup> seconds  
      = 282.551 &times; 10<sup>-3</sup> seconds  
      = 282.551 milliseconds  

#### How Long Should Power-on /RESET be?

The on-board power-on auto-reset circuit would provide between 8.2 and 24 milliseconds
(if my calculations are correct; a test with a 7404 inverter measured ~8 milliseconds).

The recommended external 555 Timer circuit provides a 110 millisecond delay.

The sample reset circuit in the CPU Technical Manual provides a delay of 1/4 to 1/3 second.

What's appropriate?

How is one to know?

In his book *Build Your Own Z80 Computer*, Steve Ciarcia gives as the reason for
an automatic power-on reset to cause the CPU to delay executing instructions
until the rest of the computer is powered up and ready to respond to CPU signals.
He mentions delays of 35-50 milliseconds for a computer that would eventually
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
The power-on auto-reset circuit does not have to provide all of the needed delay.

### Unused Z80 CPU and Z80 PIO Features

The Z80 CPU and the Z80 PIO have features that this board does not enable.
Unused inputs are tied to a high logic level (inactive) through
a 4.7K&ohm; resistor to Vcc.
Unused outputs are not connected.

#### Direct Memory Access (DMA)

The Z80 CPU will cede control of the bus to other devices
to allow them direct access to memory or I/O devices.
The low signal /BUSRQ input causes the Z80 CPU to set the address bus, the data bus,
and the control signals /MREQ, /IOREQ, /RD, /WR to a high impedance state
at the end of the current machine cycle.
The CPU signals that it has ceded the bus by activating the /BUSAK signal.

The board ties CPU pin 25 /BUSRQ to Vcc through 4.7K&ohm; resistor,
and leaves CPU pin 23 /BUSAK unconnected.

#### Halt State

The Z80 CPU indicates that it is halted, as a result of executing a HALT instruction,
by activating the /HALT signal.
The CPU will remain in the HALT state until it receives a non-maskable interrupt
or an unmasked maskable interrupt.

The board leaves CPU pin 18 /HALT unconnected.

#### Memory Refresh

The Z80 CPU uses the T3 and T4 clock states of an M1 instruction fetch cycle
to refresh dynamic memories.
It activates the /RFSH signal to indicate that the address bus has a refresh address.

The board does not have any dynamic RAMs, so it leaves CPU pin 28 /RFSH unconnected.

#### Interrupt Priority Chaining

The Z80 PIO can participate in interrupt priority logic using the 
IEI interrupt enable input and the IEO interrupt enable output signals.
The PIO will generate a pending interrupt request if IEI is high,
meaning no higher priority device is interrupting the CPU.
The PIO will generate a high signal on the IEO output if it
does not need to interrupt the CPU, enabling lower priority devices
to do so.

The board does not have any devices besides the PIO that can interrupt the CPU,
so it ties PIO pin 24 IEI to Vcc through 4.7K&ohm; resistor,
and leaves PIO pin 22 IEO unconnected.

## TEST 1 EPROM

The board I have came with a 2716 EPROM labelled TEST 1.
I was not sure that a 2716 programmed around 1980 would still be readable,
but in fact it contains only a few bits in error far from the test program,
which occupies the first 18 bytes.
The test program outputs a two-byte count, 0000..FFFF, to the two PIO ports.

### EPROM Reader

The EPROM reader, which I built with 3 shift register ICs on a solderless breadboard
and a short C program on an Arduino compatible micro-controller (a TinyDuino), read the content of
the 2716 EPROM and wrote it in hexadecimal to the serial terminal emulator on the attached PC.
From there, I could copy and paste it into a text file.
This was sufficient for my purposes, since I could convert the hex text to binary with a
separate program, if I needed to.

| [2716 EPROM Reader - breadboard &rightarrow;](/image/TinyDuino_MK2716_EPROM_Reader-Connected_to_processor.JPG) |
|----------------------------------------------------------- |
![2716 EPROM Reader - breadboard](/image/TinyDuino_MK2716_EPROM_Reader-connected.PNG "2716 EPROM Reader breadboard connected")

### EPROM Reader Circuit

The EPROM reader circuit comprises three shift registers connected to the EPROM to be read.

A 74LS95 4-bit shift register holds the high order bits of the address.
A 74LS299 8-bit shift register holds the low order bits of the address.
They are wired together to form a 12-bit address register,
and provide the 2716 EPROM with the 11 address bits it requires.

Another 74LS299 8-bit shift register holds the data read from the EPROM.

The connections to the TinyDuino microcontroller are in three groups:

| Address Register Controls | Data Register Controls      | EPROM Controls      |
| ------------------------- | --------------------------- | ------------------- |
| CLKA : Shift right 1 bit  | CLKD : Shift right or load  | /CE : Chip enable   |
| Dso  : Serial data out    | Dsi  : Serial data in       | /OE : Output enable |
|                           | LDE  : Parallel load mode   |                     |

A ground connection provides a common signal reference between the TinyDuino and
the EPROM reader circuit.
The two are powered by separate 5 volt supplies.

| [2716 EPROM Reader - schematic &rightarrow;](/image/TinyDuino_MK2716_EPROM_Reader.pdf) |
|----------------------------------------------------------- |
![2716 EPROM Reader - schematic](/image/EPROM_2716_Reader_schematic_big.png "2716 EPROM Reader")

### EPROM Reader Circuit Operation

[2716 EPROM Reader - Arduino sketch &rightarrow;](/sketches/sketch_read_2716_eprom_jul25a/sketch_read_2716_eprom_jul25a.ino)

To operate the EPROM reader circuit, the TinyDuino manipulates the control lines to
shift an address into the address register, cause the 2716 EPROM to read the byte at
that address, cause the data register to parallel load the data, and finally shift
the byte read out of the data register.
The TinyDuino repeats this read cycle for each byte to be read,
normally, all 2048 bytes.

#### Address Load

The shift registers comprising the address registered are configured so that
the low order register shifts on the rising edge of the CLKA signal, and
the high order register shifts on the falling edge.
When not loading an address, the TinyDuino keeps CLKA low and Dso does not matter.

To load the address, the TinyDuino sets the low order address bit, A0, on the Dso line,
then toggles the CLKA signal high then low.
The rising edge causes the low order register to shift right, thereby replacing
its high order bit with the low order bit of the high order register;
it shifts its own low order bit out the right end, discarding it.
The falling edge of CLKA causes the high order register to shift right, thereby replacing
its high order bit with the bit on the Dso line;
it shifts its own low order bit out the right end, discarding it,
but that bit has already been captured by the low order address register.

The TinyDuino repeats the operation 12 times to load a 12-bit address in low-to-high order,
A0..A11, into the address register, the low order 11 bits of which are the required EPROM
data byte address.

At the end of the address load operation, CLKA is again low and Dso does not matter.

#### Data Read

Once the address is loaded, the TinyDuino sets /CE low to cause the 2716 EPROM
to read the byte at that address, then /OE low so the EPROM presents the byte read
on its data lines.

The TinyDuino sets LDE high to put the data register in parallel load mode,
then toggles CLKD high then low.
The data register parallel loads the data on the 2716 EPROM's data lines
on the rising edge of CLKD.

The TinyDuino sets /OE high so that the 2716 EPROM's data lines are set in the
high impedance state.
It sets /CE high to put the 2716 EPROM in its quiescent state, not reading its data.
LDE is still high.

#### Data Input

To read the data byte now in the data register, the TinyDuino shifts it in
low order bit first on the Dsi line.

The TinyDuino sets LDE low to put the data register in right shift mode.
It reads the low order bit of the data register on the Dsi line,
then toggles CLKD high then low to shift the register right.
It repeats this 8 times to shift in the byte.

The TinyDuino sets LDE high, so that the data register's data lines are set
in the high impedance state.
CLKD was left low after the last shift, and Dsi is not receiving data from any source.
/CE and /OE are both high.
This is the normal state of LDE, CLKD, Dsi, /CE, and /OE between data reads.

This completes the read cycle for one byte.

### EPROM Content

The content read from the TEST 1 EPROM was:

```
15:32:41.785 -> Read 2716 EPROM.
15:32:41.785 -> 0000: 3E0FD302D3032100007DD3007CD30123
15:32:43.004 -> 0010: 18F7FFFFFFFFFFFFFFFFFFFFFFFFFFFF
15:32:44.223 -> 0020: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
...
15:33:12.119 -> 0190: FFFFFFFFFFFFFFFFFFFFDB98FFFFFFFF
...
15:35:14.710 -> 07E0: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
15:35:15.920 -> 07F0: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
```

The first two lines contain the test program.
The middle line, offset 0190, contains the only errors on the chip.

### Decompiling

Because the program is so short, I decompiled it by hand and ran the source
through an assembler to verify it, with this result:

```
0001   0000             ; TEST 1
0002   0000             ;    Test program for John Bell Z80 Computer 80-280.
0003   0000             ;    Output a 16-bit count on PIO ports A and B.
0004   0000             ;    Reverse engineered from EPROM on board.
0005   0000             ;
0006   0000             ; TEST 1 EPROM
0007   0000             ;
0008   0000             ; 15:32:41.785 -> Read 2716 EPROM.
0009   0000             ; 15:32:41.785 -> 0000: 3E0FD302D3032100007DD3007CD30123
0010   0000             ; 15:32:43.004 -> 0010: 18F7FFFFFFFFFFFFFFFFFFFFFFFFFFFF
0011   0000             ; ...
0012   0000             ;
0013   0000             ; Sig Nin, July 27, 2022
0014   0000             ;
0015   0000             	.ORG 	0000h
0016   0000             ;
0017   0000             MODEOUT .EQU 00001111B
0018   0000             ADATA   .EQU 0
0019   0000             BDATA   .EQU 1
0020   0000             ACTRL   .EQU 2
0021   0000             BCTRL   .EQU 3
0022   0000             ;
0023   0000             ; Set PIO Ports A and B to output mode
0024   0000 3E 0F            LD  A,MODEOUT     ; Mode control word: output mode - 00..1111
0025   0002 D3 02            OUT (ACTRL),A     ; Set port A mode
0026   0004 D3 03            OUT (BCTRL),A     ; Set port B mode
0027   0006             ; Output 16-bit count on ports: low order on A, high order on B
0028   0006 21 00 00         LD  HL,0000H      ; Set count to zero
0029   0009 7D          L1   LD  A,L           ; Low order byte ..
0030   000A D3 00            OUT (ADATA),A     ; .. on port A
0031   000C 7C               LD  A,H           ; High order byte ..
0032   000D D3 01            OUT (BDATA),A     ; .. on port B
0033   000F 23               INC  HL           ; Increment count
0034   0010 18 F7            JR   L1           ; Repeat
0035   0012             ;
0036   0012                  .END
```

