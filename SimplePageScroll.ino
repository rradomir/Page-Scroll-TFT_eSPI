/*  
 Simple scrollable page

 This sketch used font  4
 
 Make sure all the required fonts are loaded by editing the
 User_Setup.h file in the TFT_eSPI library folder.
 
 Simple page scrolling with scrollbar

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
 */

#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

TFT_eSprite window = TFT_eSprite(&tft); 

#define WINDOW_HEIGHT 420
#define WINDOW_LENGTH 1600

#define SCROLL_TOP 50
#define SCROLL_WIDTH 50
#define SCROLL_LEFT 270
#define SCROLL_HEIGHT 430
#define SCROLL_BACK 0x0200
#define SCROLL_FRONT TFT_DARKGREEN
#define SCROLL_PRESSED TFT_GREEN

uint16_t scrollLength;
int16_t scrollHandleTop=0; 
uint16_t scrollMax;

void setup(void) {
//for better my screen performance
// Get the processor sys_clk frequency in Hz
 
	uint32_t freq = clock_get_hz(clk_sys);

 // clk_peri does not have a divider, so input and output frequencies will be the same
	clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    freq,
                    freq);


	tft.init();
	tft.setRotation(0);

	uint16_t calData[5] = { 358, 3319, 303, 3539, 6 }; //my calibration
	tft.setTouch(calData);

	tft.fillScreen(0);
	tft.fillRect(0,0,320,50,TFT_BLUE); // main menu mockup

	window.setColorDepth(4);
	window.createSprite(250, WINDOW_LENGTH);
	
	for (uint8_t i=0;i<16;i++) //draw more mockups
	{
		window.drawRect(0,i*100,250,50,TFT_WHITE);
		window.fillRect(1,i*100+1,random(10,249),48,i);
		window.drawNumber(i,10,i*100+65,4);
	}

	scrollLength=SCROLL_HEIGHT*((float)WINDOW_HEIGHT / (float)WINDOW_LENGTH);
	scrollMax=WINDOW_LENGTH-WINDOW_HEIGHT;
}

uint16_t x = 0, y = 0; 
bool pressed;
int16_t scroll=0;
int16_t lastScroll=-1;
bool scrollPressed=false;
uint16_t scrollPressedStart;

void loop() {
	pressed = tft.getTouch(&x, &y);
	if (pressed)
	{
		if (scrollPressed || (x > SCROLL_LEFT && y > SCROLL_TOP)) // assumed scroll is far bottom right, so no checking for x and y second boundary
		{

			if (y > SCROLL_TOP + scrollHandleTop && y < SCROLL_TOP + scrollHandleTop + scrollLength && !scrollPressed)
			{ //first touch of scroll
				scrollPressed=true;
				redrawScroll(); //recolor of scroll
				scrollPressedStart=y-(SCROLL_TOP + scrollHandleTop); //store offset of touched y and top of scroll
			}
			else { //touched outside scroll handle, so moving one page up pr down
				scrollHandleTop+=scrollLength * (y - SCROLL_TOP < scrollHandleTop?-1:1);
				calculateScrollTop();
			}
			if (scrollPressed) //draging scroll
			{ 
				scrollHandleTop=y-scrollPressedStart; //use offset to calculate position
				calculateScrollTop();
			}
		}
	}
	else if (scrollPressed)	//release scroll
	{
		scrollPressed=false;
		redrawScroll(); //recolor of scroll
	}
	if (scroll!=lastScroll) //position was changed
	{
		lastScroll=scroll;
		window.pushSprite(10,60,0,scroll,250,WINDOW_HEIGHT);
		redrawScroll();
	}
}


void redrawScroll(){
	if (scrollHandleTop>0) //back above handle
		tft.fillRect(SCROLL_LEFT,SCROLL_TOP,SCROLL_WIDTH,scrollHandleTop,SCROLL_BACK);
	tft.fillRect(SCROLL_LEFT,SCROLL_TOP+scrollHandleTop,SCROLL_WIDTH,scrollLength,scrollPressed?SCROLL_PRESSED:SCROLL_FRONT); //handle
	
	if (SCROLL_HEIGHT-scrollLength-scrollHandleTop>0) //back under handle
	tft.fillRect(SCROLL_LEFT,SCROLL_TOP+scrollHandleTop+scrollLength,
		SCROLL_WIDTH,SCROLL_HEIGHT - scrollLength - scrollHandleTop,SCROLL_BACK);
}

void calculateScrollTop(){
	scrollHandleTop=constrain(scrollHandleTop,0,SCROLL_HEIGHT - scrollLength ); //check boundaries
	scroll=((float)scrollHandleTop / (SCROLL_HEIGHT - scrollLength) ) * scrollMax; //calculate current top of main window
}