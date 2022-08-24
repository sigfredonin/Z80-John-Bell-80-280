/*
 * TinyDuino MK2716T EPROM Reader
 * 
 * The MK2716T EPROM Reader consists of an SN74LS95 shift
 * register to hold the high order 4 bits of a 12 bit address, 
 * an SN74LS299 shift register to hold the low order 8 bits 
 * of the address. Together they form the address register. 
 * Another SN74LS299 shift register holds the data byte 
 * read from the EPROM; it comprises the data register.
 * 
 * In normal operation, the TinyDuino shifts the address bits
 * into the address register, low order bit first, enables
 * the EPROM to read the requested byte, loads the byte into
 * the data register, then shifts the data byte into the
 * TinyDuino, low order bit first.
 * 
 * This reader sketch reads all of the EPROM content, 16 bytes at a time,
 * and writes the offset and the bytes in hex to the serial terminal.
 * When it is done, it lights up the TinyDuino's LEDs in a back and forth
 * sweep pattern.
 *
 * 2022 July 17
 * Sigfredo I. Nin
 * signin@email.com
 */

/* ----------------------------------------------------------- */
/* EPROM READER ---------------------------------------------- */
/* ----------------------------------------------------------- */

// For testing, set a delay after changing digital signal
unsigned int delaymsecs = 1;

// Define which  pins the control signals are on
unsigned int LDE  = A5; // Data Register mode: 0=shift, 1=load
unsigned int CLKD = A4; // Data Register strobe
unsigned int _OE  = A3; // EPROM output enable (active low)
unsigned int _CE  = A2; // EPROM chip enable (active low)
unsigned int CLKA = A1; // Address Register strobe
unsigned int Dsi  = A0; // Data Register serial input
unsigned int Dso  = 4;  // Address Register serial output on pin 4

// Set the pin modes to digital outputs; set initial values
void resetPorts() {
  pinMode(LDE, OUTPUT);     // Set Data Register to LOAD mode
  digitalWrite(LDE, HIGH);
  pinMode(_OE, OUTPUT);     // Disable EPROM outputs
  digitalWrite(_OE, HIGH);
  pinMode(_CE, OUTPUT);     // Disable EPROM
  digitalWrite(_CE, HIGH);
  pinMode(CLKD, OUTPUT);    // Set Data Clock low
  digitalWrite(CLKD, LOW);
  pinMode(CLKA, OUTPUT);    // Set Address Clock low
  digitalWrite(CLKA, LOW);
  pinMode(Dso, OUTPUT);     // Set Address[0] = 0
  digitalWrite(Dso, LOW);
  pinMode(Dsi, INPUT);      // Data Register input
}

// Shift address into Address Register
void loadAddressRegister(unsigned int address) {
  for (int bit=0; bit < 12; bit++) {
    digitalWrite(Dso, address & 0x1);
    digitalWrite(CLKA, HIGH);
    delay(delaymsecs);
    digitalWrite(CLKA, LOW);
    delay(delaymsecs);
    address >>= 1;
  }
  digitalWrite(Dso, LOW);
  // Exit with CLKA=LOW, Dso=LOW
}

// Load data from EPROM into Data Register
// Desired address must be in Address Register
void readEPROM() {
  digitalWrite(LDE, HIGH);  // Set Data Register to LOAD mode
  delay(delaymsecs);
  digitalWrite(_CE, LOW);   // Enable EPROM
  delay(delaymsecs);
  delay(delaymsecs);
  digitalWrite(_OE, LOW);   // Enable EPROM output
  delay(delaymsecs);
  digitalWrite(CLKD, HIGH); // Clock data into Data Register
  delay(delaymsecs);
  digitalWrite(CLKD, LOW);
  delay(delaymsecs);
  digitalWrite(_OE, HIGH);  // Disable EPROM output
  delay(delaymsecs);
  digitalWrite(_CE, HIGH);  // Disable EPROM
  delay(delaymsecs);
  // Exit with LDE=HIGH, _CE=HIGH, _OE=LOW, CLKD=LOW
}

// Shift data out of Data Register
unsigned int getData() {
  unsigned int dataByte = 0;
  unsigned int dataBit;
  digitalWrite(LDE, LOW);  // Set Data Register to SHIFT mode
  delay(delaymsecs);
  for (int bit=0; bit < 8; bit++) {
    dataBit = digitalRead(Dsi);
    dataBit <<= bit;
    dataByte |= dataBit;
    digitalWrite(CLKD, HIGH);
    delay(delaymsecs);
    digitalWrite(CLKD, LOW);
    delay(delaymsecs);
  }
  digitalWrite(LDE, HIGH);  // Set Data Register to LOAD mode
  delay(delaymsecs);
  // Exit with LDE=HIGH, CLKD=LOW
  return dataByte;
}

// Read the byte at the given address from the EPROM
unsigned int readByteAtAddress(unsigned int address) {
  loadAddressRegister(address);
  readEPROM();
  return getData();
}

/* ----------------------------------------------------------- */
/* 16-LED ROUTINES ------------------------------------------- */
/* ----------------------------------------------------------- */

void ledInt(int c) {
  for (int i = 0; i < 16 ; i++) {
    LedOn(0);
    if ((c >> (15-i)) & 1)
      LedOn(1+i);
    delay(1);
  }
}

void sweep_CW()
{
  for(int i = 0; i < 16 ;i++) {
    LedOn(1+i); // LED 1,2,..,16  (1+0..1+15)
    delay(20);
  };
  LedOn(0);
}

void sweep_CCW()
{
  for (int i = 0; i < 16 ;i++) {
    LedOn(16-i);  // LED 16,15,..,1 (16-0..16-15)
    delay(20);
  };
  LedOn(0);
}

void LedOn(int ledNum)
{
  for (int i = 5; i < 10; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  };
  if (ledNum < 1 || ledNum > 16)
    return;
  char highpin[16] = {5,6,5,7,6,7,6,8,5,8,8,7,9,7,9,8};
  char lowpin[16] = {6,5,7,5,7,6,8,6,8,5,7,8,7,9,8,9};
  ledNum--;
  digitalWrite(highpin[ledNum], HIGH);
  digitalWrite(lowpin[ledNum], LOW);
  pinMode(highpin[ledNum], OUTPUT);
  pinMode(lowpin[ledNum], OUTPUT);
}

/* ----------------------------------------------------------- */
/* PRINT HEX DATA -------------------------------------------- */
/* ----------------------------------------------------------- */

// Print stored data as one hex string, 2 characters per byte
void printDataRow(unsigned char data[]) {
  for (int i = 0; i < 16; i++) {
    if ((data[i] & 0x0F0) == 0) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
  }
}

// Print address (0000..FFFF) as 4-character hex string
void printAddress(unsigned int address) {
  address &= 0x0FFFF;
  if ((address & 0x0F000) == 0) {
    Serial.print("0");
  }
  if ((address & 0x0FF00) == 0) {
    Serial.print("0");
  }
  if ((address & 0x0FFF0) == 0) {
    Serial.print("0");
  }
  Serial.print(address, HEX);
}
/* ----------------------------------------------------------- */
/* SETUP & LOOP ---------------------------------------------- */
/* ----------------------------------------------------------- */

void setup() {
  resetPorts();
  LedOn(0); //Pass a zero to turn all LEDs off
  Serial.begin(9600);
  delay(5000);
  Serial.println();
  Serial.println("Read 2716 EPROM.");
}

unsigned int byteAddress = 0;
unsigned int byteCount = 0;
unsigned char data[16];
void loop() {
  // Read 16 bytes at a time ...
  if (byteAddress < 2048) {
    printAddress(byteAddress);
    Serial.print(": ");
    for (int i = 0; i < 16; i++) {
      data[i] = readByteAtAddress(byteAddress);
      byteAddress++;
    }
    printDataRow(data);
    Serial.println();
  } else {
    sweep_CCW();
    sweep_CW();
  }
}
