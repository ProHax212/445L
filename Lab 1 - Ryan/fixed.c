/*
Author: RYAN COMER
Date: 08/31/2016
This is the implementation for Lab 1.  This file implements 4 functions:
1) ST7735_sDecOut3 - print the decimal fixed point representation of the input (scaling factor = 1/1000)
2) ST7735_uBinOut8 - print the binary fixed point representation of the input (scaling factor = 1/256 = 1/(2^8))
3) ST7735_XYplotInit - initialize the LCD screen to plot data by setting the minX/maxX/minY/maxY values that will be read
4) ST7735_XYplot - plot the x/y data from the x and y arrays (assumes that ST7735_XYplotInit was already called)
*/

#include <stdint.h>
#include <stdio.h>
#include "fixed.h"
#include "ST7735.h"

/*
Converts fixed point number to LCD format signed 32-bit with resolution 0.001 (scale = 1/1000)
range -9.999 to +9.999
Parameter LCD display
*/
void ST7735_sDecOut3(int32_t n){
	// Input out of range - print error (" *.***")
	if(n > 9999 || n < -9999){
		printf(" *.***");
		return;
	}
	
	if(n >= 0) printf(" ");	// Print an extra space if the number is positive (keep 6 characters total)
	printf("%.3f", (float)n / 1000);	// Print the fixed point number (3 decimal precision)
}

/*
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD.
 The scaling factor is 1/256
*/
void ST7735_uBinOut8(uint32_t n){
	// Error ocurred - print the error message
	if(n > 256000){
		printf("***.**");	// Print error message: ***.**
		return;
	}
	
	// Store the binary fixed point representation into a character array (2 decimal places)
	char number[6];
	int numChar = sprintf(&number[0], "%.2f", (float)n / 256);	// numChar is the number of characters returned
	
	int numSlotsLeft = 6 - numChar;	// Number of empty slots - for shifting the output
	
	// Print a space for each empty slot (6 - len(character_array))
	int i = 0;
	for(i = 0; i < numSlotsLeft; i++){
		printf(" ");
	}
	
	printf("%s", number);	// Print the actual number
}

/*
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 assumes minX < maxX, and miny < maxY
*/
static float minXVar = 0, maxXVar = 127, minYVar = 0, maxYVar = 159;	// Variables for drawing window (The bounds)
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	// Initialize the screen
	ST7735_InitR(INITR_REDTAB);
	
	// Set the bounds on the static variables
	minXVar = minX;
	maxXVar = maxX;
	minYVar = minY;
	maxYVar = maxY;
	
	ST7735_PlotClear(0,4096);  // Clear the plot area
	
	// Output the title
  ST7735_OutString(title);
	ST7735_OutString("\n");
}

/*
 Plot an array of (x,y) data
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int i = 0, offsetX = 0, offsetY = 0;	// Offset is only used if the min values are negative.  Negative numbers throw off the scaling
	if(minXVar < 0) offsetX = (0 - minXVar);
	if(minYVar < 0) offsetY = (0 - minYVar);
	
	// Loop through the points and plot if within range
	for(i = 0; i < num; i++){
		int x = bufX[i], y = bufY[i];
		
		// Numbers out of range -> continue
		if(x < minXVar || x > maxXVar) continue;
		if(y < minYVar || y > maxYVar) continue;
		
		// Use the compression to find the real point (i.e. maxX -> 127, minX -> 0 | minY -> 159, maxY -> 32)
		int realX = (x + offsetX) * (127 / (maxXVar - minXVar));
		int realY = ((y + offsetY) * (127 / (maxYVar - minYVar))) + 32;
		
		// Flip the y value, otherwise would be drawn upside down
		int midPoint = (159 + 32) / 2;
		realY -= 2 * (realY - midPoint);
		
		// Draw the point
		ST7735_DrawPixel(realX, realY, ST7735_Color565(0, 0, 0));
	}
}
