/* 
 * File:   main.c
 * Author: Erkin Oto
 *
 * Created on April 23, 2019, 1:24 AM
 */

#include <proc/p32mx250f128b.h>
#include <xc.h>
#include<sys/attribs.h>  // __ISR macro

#include "i2c_master_noint.h"
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module
// Demonstrate I2C by having the I2C1 talk to I2C5 on the same PIC32 (PIC32MX795F512H)
// Master will use SDA1 (D9) and SCL1 (D10).  Connect these through resistors to
// Vcc (3.3 V) (2.4k resistors recommended, but around that should be good enough)
// Slave will use SDA5 (F4) and SCL5 (F5)
// SDA5 -> SDA1
// SCL5 -> SCL1
// Two bytes will be written to the slave and then read back to the slave.
#define SLAVE_ADDR 0x20

void initExpander();
//write to the expander
void setExpander(char pin, char level);
//read from the expander
char getExpander();


int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
   __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
  
    TRISAbits.TRISA4 = 0; // Pin 4 of Port A is LED1
    TRISBbits.TRISB4 = 1; // Pin 4 of Port B is input button
    LATAbits.LATA4 = 1; // Turn on LED1 ON
    initExpander();
    setExpander(0,1);
    
    __builtin_enable_interrupts();
    char value;
   
    while(1) {
    
    _CP0_SET_COUNT(0); 
    while(_CP0_GET_COUNT() < 4800000){  //
    LATAbits.LATA4 = 0;     // turn off/on LED
            
    }
    
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < 4800000){ //
    LATAbits.LATA4 = 1;
            
    } 
    value = getExpander()>>7;
    setExpander(0,value);
    
}
    return 0;
}


void initExpander() {
    
   
    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1|0);
    i2c_master_send(0x00); 
    i2c_master_send(0xF0); //send to IODIR
    i2c_master_stop();
    
    
}

void setExpander(char pin, char level) {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1|0);
    i2c_master_send(0x0A);
    i2c_master_send(level<<pin);
    i2c_master_stop();
    

}

char getExpander() {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR<<1|1);
    char recv = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return recv;
}

