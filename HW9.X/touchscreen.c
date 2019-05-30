#include <xc.h>
#include "touchscreen.h"
#include<math.h>
#include "ili9341.h"
#include<string.h>

void read_XPT2046(unsigned short *x, unsigned short *y, unsigned int *z) {
    unsigned short temp1, temp2, z1, z2;
        

    CS2 = 0; 
    //0b 1 A2A1A0 0001
    
        //get z1 pressure: A2 = 0, A1 = 1, A0 = 1
    spi_io(0b10110001); 
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    CS2 = 1;
    z1 = ((temp1  << 8) | temp2)>>3;
    CS2 = 0;
    
    //get z2 pressure: A2 = 1, A1 = 0, A0 = 0
    spi_io(0b11000001); 
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    z2 = ((temp1  << 8) | temp2)>>3;
    CS2 = 1;
    *z = z1 - z2 + 4095; 
    CS2 = 0;
    
    //get x position: A2 = 1, A1 = 0    , A0 = 1
    spi_io(0b11010001);
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    CS2 = 1;
    *x = ((temp1  << 8) | temp2)>>3;  
    CS2 = 0;
    //get y position: A2 = 0, A1 = 0, A0 = 1    
    spi_io(0b10010001);
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);

    *y = ((temp1  << 8) | temp2)>>3; 
    


    CS2 = 1; 
}

unsigned short x_Pixel(unsigned short x) {
    unsigned short xScaled = ((x-460.0)/15.0); //0->460, 240->4095
    return xScaled;
}

unsigned short y_Pixel(unsigned short y) {
    unsigned short yScaled = ((y-420.0)/10.5); //0->420, 310->3870
    return yScaled;
}

void buttons(void) {
    char message[100], i, j;
    //button I++
    for (i=0;i<40;i++) {        //length
        for (j=0;j<20;j++) {    //height
            LCD_drawPixel(120+i, 160+j, ILI9341_BLUE);
        }
    }
    sprintf(message, "I++");
    LCDprint(message, 120+13, 160+8, ILI9341_YELLOW, ILI9341_BLUE);
    
       //button I--
    for (i=0;i<40;i++) {        //length
        for (j=0;j<20;j++) {    //height
            LCD_drawPixel(120+i, 230+j, ILI9341_BLUE);
        }
    }
    sprintf(message, "I--");
    LCDprint(message, 120+13, 230+8, ILI9341_YELLOW, ILI9341_BLUE);
}

signed int push_button(unsigned short xPixel, unsigned short yPixel) {
    if ((xPixel>=117) && (xPixel <=152)) {
        if ((yPixel>=144) && (yPixel<=164)) {
            return 1;
        }
        else if ((yPixel>=71) && (yPixel<=88)) {
            return -1;
        }
    }
}
