// filename ******** fixed.c **************
// Brian Connor White - bcw824
// Ryan Comer - rnc525
// Initial Creation Date: 09/01/2016
// Lab: 1
// TA: Dylan Zika
// Last Revision: 09/05/2016


#include "fixed.h"

int32_t xMin, xMax, yMin, yMax;

/****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */
void ST7735_sDecOut3(int32_t n){
	if(n<0)//Check for negative vs positive
		printf("-");
	else
		printf(" ");
	int32_t absN = abs(n);
	
	if(absN >= 10000)
		printf("*.***");
	else
		printf("%1d.%0.3d", absN/1000, absN%1000);
}


/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     2	  "  0.01"
    64	  "  0.25"
   100	  "  0.39"
   500	  "  1.95"
   512	  "  2.00"
  5000	  " 19.53"
 30000	  "117.19"
255997	  "999.99"
256000	  "***.**"
*/
void ST7735_uBinOut8(uint32_t n){
	if(n >= 256000)
		printf("***.**");
	else
		printf("%3d.%0.2d", n/256, ((n%256)*100)/256);
}	

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	xMin = minX;
	xMax = maxX;
	yMin = minY;
	yMax = maxY;
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t domain = xMax - xMin;
	int32_t range = yMax - yMin;
	
	for(int p=0; p<num; p++){
		int32_t x = bufX[p];
		int32_t y = bufY[p];
		
		if(x < xMax && x > xMin && y > yMin && y < yMax)
			ST7735_DrawPixel(((x-xMin)*ST7735_TFTWIDTH/domain), 32+ST7735_TFTWIDTH-((y-yMin)*ST7735_TFTWIDTH/range), ST7735_YELLOW);
	}
}


