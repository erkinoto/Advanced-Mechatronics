 /* File:   main.c
 * Author: Erkin Oto
 *
 * Created on April 24, 2019, 9:46 PM
 */

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include"ili9341.h"
#include <math.h>
#include "i2c_master_noint.h"
#include "LSM6DS33.h"



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


#define LENGTH 14



//#define OUT_TEMP_L 0x20

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
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LATAbits.LATA4 = 1;

    ANSELBbits.ANSB2 = 0;
	ANSELBbits.ANSB3 = 0;

    __builtin_enable_interrupts();


    SPI1_init();
    LCD_init();
    imu_init();
    LCD_clearScreen(ILI9341_PURPLE);
    char message[100];
    unsigned char data[LENGTH]; //14
    signed short aX, aY;
    while(1) {
        //Read LCM at 20 Hz
        LATAbits.LATA4 = 0;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<=1200000){;} //delay
        LATAbits.LATA4 = 1;

        //WHO AM I
        signed char input = whoAmI();
        sprintf(message, "Who Am I = %d (105)", input);
        LCDprint(message, 28,32 , ILI9341_WHITE, ILI9341_PURPLE);

        //Let's get some readings from IMU
        I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, LENGTH);
        aX = (data[9]  << 8) | data[8];
        aY = (data[11] << 8) | data[10];

        signed char Xaxis = ((int)aX/165.0);
        signed char Yaxis = ((int)aY/165.0);
        sprintf(message, "Xaxis = %d  ", Xaxis);
        LCDprint(message, 28, 72, ILI9341_YELLOW, ILI9341_PURPLE);
        sprintf(message, "Yaxis = %d  ", Yaxis);
        LCDprint(message, 28, 82, ILI9341_YELLOW, ILI9341_PURPLE);

        //Let's draw bars:
        Xbar(120, 160, 100, Xaxis, ILI9341_BLUE, ILI9341_YELLOW);
        Ybar(120, 160, 100, Yaxis, ILI9341_BLUE, ILI9341_YELLOW);
    }
}
