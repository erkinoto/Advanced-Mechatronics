#include"SPI1.h"

// send a byte via spi and return the response
unsigned char SPI1_IO(unsigned char ch) {
    SPI1BUF = ch;
    while(!SPI1STATbits.SPIRBF) {;} // wait to receive the byte
    return SPI1BUF;
}

// initialize SPI1
void SPI1_init() {
    // Master - SPI1, pins are: SDI1(B8), SDO1(A1), SCK1(B14).  
    // we manually control SS1 as a digital output (A0)
    RPA1Rbits.RPA1R = 0b0011; // Set SDO1 to A1
    SDI1Rbits.SDI1R = 0b0100; // Set SDI1 to B8, actually not connected to DAC
    SPI1CON = 0;              // turn off the SPI module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x1;            // SPI1BRG = (48MHz/(2*x MHz))-1 = 1; now x = 12MHz
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from HIGH to LOW (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on SPI1
    TRISAbits.TRISA0 = 0;
}


//where channel is 0 or 1 (for VoutA and VoutB), and voltage is the 8-bit output level.
void setVoltage(char a, int v){
    unsigned char LSB,MSB;
    unsigned short t=0;
    t= a << 15; //a is at the very end of the data transfer
	t = t | 0b0111000000000000;
    t = t | (v&0b111111111111);
    LSB = t;
    //The first 4 bits as the last 4 bits of MSB              
    MSB = (t >>8);
    CS = 0;                        // enable SPI1 by set chip select line low
    SPI1_IO(MSB);
    SPI1_IO(LSB);      
    CS = 1;                        // raise the chip select line, ending communication
}
