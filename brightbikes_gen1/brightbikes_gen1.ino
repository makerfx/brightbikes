
//todo: animated gifs - https://github.com/pixelmatix/AnimatedGIFs/blob/master/examples/SmartMatrixGifPlayer/SmartMatrixGifPlayer.ino
//todo: gfx library - look here for info: https://github.com/marcmerlin/Framebuffer_GFX + https://github.com/marcmerlin/FastLED_NeoMatrix

#include "brightbikes.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


bool debugOptions[10] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0};   //change default here, helpful for startup debugging

uint8_t aniMode = 8;
                                                        
const char *debugOptionsText[10] =  {"", "Input","Audio", "Action", "Peak Audio",
                                "Animation","Animation RGB Dump"};
                                
#define DEBUG_INPUT               1  //input functions 
#define DEBUG_AUDIO               2  //audio functions 
#define DEBUG_ACTION              3  //action functions 
#define DEBUG_PEAK                4  //Peak Audio functions 
#define DEBUG_ANIMATION           5  //Animation functions   //THIS MAY SLOW ANIMATION                            
#define DEBUG_ANIMATION_RGB       6  //Show Full Animation Frame RGB 
                                     //Does not require DEBUG_ANIMATION to be on
                                     //THIS MAY SLOW ANIMATION



//fire effect variables
int scalexy = 60; // scale of fire
int speed = 20; //speed of effect

DEFINE_GRADIENT_PALETTE(firepal) { // define fire palette
  0,  0,  0,  0, //black
  32,  255,  0,  0, // red
  190,  255,  255,  0, //yellow
  255,  255,  255,  255 // white
};
//end fire effect

void setup() {
 // FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);

 FastLED.addLeds<CHIPSET, LED_PIN_FRONT, COLOR_ORDER>(leds,  numPanelLEDs[0]).setCorrection(TypicalSMD5050);
 FastLED.addLeds<CHIPSET, LED_PIN_MID,   COLOR_ORDER>(ledsM, numPanelLEDs[1]).setCorrection(TypicalSMD5050);
 FastLED.addLeds<CHIPSET, LED_PIN_REAR,  COLOR_ORDER>(ledsR, numPanelLEDs[2]).setCorrection(TypicalSMD5050);
 FastLED.setBrightness( BRIGHTNESS );

 Serial.begin(115200);
 delay(100);
 
 //display init
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("OLED begun");
  
  // Clear the buffer
  display.clearDisplay();

    // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(__FILENAME__);
  display.println("Initializing...");
  display.display(); // actually display all of the above
  delay(1000);
  aniTestSweep();
  Serial.println("Setup Finished.");
  printDebugOptions();
  
  delay(1000); 
  display.clearDisplay();

  
}



/*
 * XY - this function does the pixel mapping
 * todo: fix the reversed x & y
 */
//uint16_t XY( uint8_t a, uint8_t x, uint8_t y)
int XY(uint8_t x, uint8_t y)
{

  //safety checks
  if( x >= kMatrixWidth) { 
    Serial.print("XY Safety Check: X = ");
    Serial.println(x);
    return -1;
  }
  if( y >= kMatrixHeight) {
    Serial.print("XY Safety Check: Y = ");
    Serial.println(y);
    return -1;
  }
  
  uint16_t i = (x * kMatrixHeight) + y;
  int j = 0;

  j = XYTable[i];  
                  
  return j;
 
}


int testX = 0; 
int testY = 0;

void loop()
{

   
   switch (aniMode) {
    case 0: aniFire(); break;
    case 1: FastLED.setBrightness( BRIGHTNESS );      aniSinPlasma(1); break;
    case 2: FastLED.setBrightness( BRIGHTNESS );      aniSinPlasma(2); break;
    case 3: FastLED.setBrightness( BRIGHTNESS );      aniSinPlasma(3); break;
    case 4: FastLED.setBrightness( BRIGHTNESS );      aniSinPlasma(4); break;
    case 5: FastLED.setBrightness( BRIGHTNESS );      aniXYMatrixHueShift(); break;
    case 6: FastLED.setBrightness( BRIGHTNESS );      aniTestCrawl(); break;
    case 7: FastLED.setBrightness( BRIGHTNESS );      aniTestSweep(); break;
    case 8: FastLED.setBrightness( BRIGHTNESS*1.5);   swirl(); break;
    default: aniMode = 0;
   }
    

  EVERY_N_MILLISECONDS(100) {                           
    updateOLED();
  }

  FastLED.show();
  display.clearDisplay();

  
  debugOptionsCheck();   
  
}

void aniTestCrawl() {
    /*
    Serial.print(testX);
    Serial.print(",");
    Serial.print(testY);
    Serial.print(":");
    Serial.println(XY(testY,testX));
    */
    leds[XY(testX, testY)] = ColorFromPalette (RainbowColors_p,(millis()/20)%255,255);

    
    //setLEDVal(testX,testY, CHSV(100, 255, 255));
    
    testY = testY+1; 
    if (testY> kMatrixHeight-1) {
      testY = 0;
      testX +=1;
    }
    
    if (testX> kMatrixWidth) {
      testX = 0;
      testY = 0;
    }

}

void aniTestSweep() {
  //horizontal sweep left to right
  for (int i=0; i<LED_COLS; i++) {
    for (int j=0; j<LED_ROWS; j++) {
      leds[XY(i,j)] = ColorFromPalette (RainbowColors_p,(millis()/10)%255,255);
    }
    FastLED.show();
    debugOptionsCheck(); 
    FastLED.clear();
  }

  //horizontal sweep right to left
  for (int i=LED_COLS-1; i>=0; i--) {
    for (int j=0; j<LED_ROWS; j++) {
      leds[XY(i,j)] = ColorFromPalette (RainbowColors_p,(millis()/10)%255,255);    
    }
    FastLED.show();
    debugOptionsCheck(); 
    FastLED.clear();
  }

  //vertical sweep bottom to top
  for (int i=0; i<LED_ROWS; i++) {
    for (int j=0; j<LED_COLS; j++) {
      leds[XY(j,i)] = ColorFromPalette (RainbowColors_p,(millis()/10)%255,255);
    }
    FastLED.show();
    debugOptionsCheck();
    delay(20); 
    FastLED.clear();
  }

 //vertical sweep top to bottom
  for (int i=LED_ROWS-1; i>=0; i--) {
    for (int j=0; j<LED_COLS; j++) {
      leds[XY(j,i)] = ColorFromPalette (RainbowColors_p,(millis()/10)%255,255);
    }
    FastLED.show();
    debugOptionsCheck();
    delay(20); 
    FastLED.clear();
  }
}

/*
 * sinPlasma() - colorful plasma effect
 * from:  https://editor.soulmatelights.com/gallery/472
 * todo: param that adjusts the effect
 * 
 */
void aniSinPlasma(byte effect) {
 

  static float t = 0.0;

  for (byte i = 0; i < kMatrixWidth; i++) {
    for (byte j = 0; j < kMatrixHeight; j++) {
      byte ind = 0;
     switch (effect) {
      case 1: ind = sin8(i*8+sin8(i*2+6*t))/2+sin8(j*8+sin8(j*2+t*7)/2); break;
      case 2: ind = sin8(i*16*sin(PI*j/32+t)+j*8*cos(2*PI*i/32+t*2))>>1; break;
      case 3: ind = sin8(i*j)/2-sin8(i*8+t)/2+sin8(i*i+j*j+t)/2; break;
      case 4: ind = sin8((i<<4)+t)/2+sin8((j<<4)+t)/2; break;
      default: ind = sin8((i<<4)+t)/2+sin8((j<<4)+t)/2; break;
     }
     
     CRGB val = ColorFromPalette (RainbowColors_p,(ind+millis()/20)%255,255);
     leds[XY(i,j)] = val;
    }
  }
  t+=0.1;
  
}



/*
 * fire() - flame effect
 * from: https://editor.soulmatelights.com/gallery/234
 */
void aniFire() {

 CRGBPalette16 myPal = firepal;
 
 uint16_t a = millis();
  for (int i = 0; i < LED_COLS; i++) {
    for (int j = 0; j < LED_ROWS; j++) {
      leds[XY((LED_COLS - 1) - i, (LED_ROWS - 1) - j)]  = ColorFromPalette(myPal, qsub8(inoise8(i * scalexy, j * scalexy + a, a / speed), abs8(j - (LED_ROWS - 1)) * 255 / (LED_ROWS - 1)), BRIGHTNESS);
    }
  }
  blur2d( leds, kMatrixWidth, kMatrixHeight, 5);

}


/*
 * swirl()
 * from: https://gist.github.com/kriegsman/5adca44e14ad025e6d3b
 * this will cause XY to throw safety checks
 * 
 */
 
 void swirl() {
  const uint8_t kBorderWidth = 1;
  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = beatsin8(2,10,255);
  blur2d( leds, 62, 27, blurAmount);

  // Use two out-of-sync sine waves
  uint8_t  i = beatsin8( 27, kBorderWidth, 16-kBorderWidth);
  uint8_t  j = beatsin8( 41, kBorderWidth, 16-kBorderWidth);
  // Also calculate some reflections
  uint8_t ni = (31-1)-i;
  uint8_t nj = (31-1)-j;
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  

  leds[XY( i, j)] += CHSV( ms / 11, 200, 255);
  leds[XY( j, i)] += CHSV( ms / 13, 200, 255);
  leds[XY(ni,nj)] += CHSV( ms / 17, 200, 255);
  leds[XY(nj,ni)] += CHSV( ms / 29, 200, 255);
  leds[XY( i,nj)] += CHSV( ms / 37, 200, 255);
  leds[XY(ni, j)] += CHSV( ms / 41, 200, 255);

  uint8_t offset = 31;
  leds[XY( i+offset, j)] += CHSV( ms / 11, 200, 255);
  leds[XY( j+offset, i)] += CHSV( ms / 13, 200, 255);
  leds[XY(ni+offset,nj)] += CHSV( ms / 17, 200, 255);
  leds[XY(nj+offset,ni)] += CHSV( ms / 29, 200, 255);
  leds[XY( i+offset,nj)] += CHSV( ms / 37, 200, 255);
  leds[XY(ni+offset, j)] += CHSV( ms / 41, 200, 255);
 }

/*
 * XYMatrixHueShift()
 * from the XYMatrix FastLED example
 */
void aniXYMatrixHueShift() {
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27) ) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39) ) * (310 / kMatrixHeight));
    
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);

}

/*
 * Helper function for XYMatrixHueShift() - XYMatrix FastLED example 
 */
void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      //Serial.print(x);
      //Serial.print(",");
      //Serial.println(y);
      leds[XY(x, y)] =  CHSV( pixelHue, 255, 255);
     
    }
  }
}

/*
 * updateOLED()
 * 
 */
void updateOLED() {
  display.setCursor(80,0);
  display.print("FPS:");
  display.print(FastLED.getFPS());
  
  for (int i=0; i<LED_COLS; i++) {
   for (int j=0; j<LED_ROWS; j++) {
      CRGB val = leds[XY(i,j)];
      //check to see if pixel is mapped, and then sample the red channel 
      if ((XY(i,j) > 0) && (val.red > 10)) {
        //we skip every other horizontal pixel for better aspect ratio
        display.drawPixel(2*i, SCREEN_HEIGHT - j, SSD1306_WHITE);
      }
    }
  }
 
  display.display();

}


/*
 * debugOptionsCheck() - this function checks the Serial input and
 *                       changes debug options as well as emulates some 
 *                       button presses
 *                       
 */
void debugOptionsCheck() {
  int incomingByte = 0;
  
  if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();
      int option;
      
      switch (incomingByte) {
          case '0':                         //turn all options off
            for (int o = 1; o<10; o++) {    //we dont use zero
              debugOptions[o] = 0;
            }
            Serial.println ("All debug options turned OFF");
            break;
            
          case '1' ... '9': 
            option = incomingByte - '0';
            debugOptions[option] = !debugOptions[option]; 
            //Serial.printf("Debug option %d is now %s\n", option, debugOptions[option]?"ON":"OFF");
            break;

          case 'q': FastLED.clear(); aniMode++; break;
          //case 'w': mapAction(SOURCE_BUTTON, 1, 0); break;
          //case 'e': mapAction(SOURCE_BUTTON, 2, 0); break;
          //case 'r': mapAction(SOURCE_BUTTON, 3, 0); break;
          }
         
         printDebugOptions();
          
      }
       
}

/*
 * printDebugOptions() - this function outputs a debug option list with
 *                       with current status, and provides some input 
 *                       instructions
 *  
 */
void printDebugOptions() {
  Serial.println("\nDebug Options Status");
  Serial.println("Use serial input keys 1 through 9 to change debug options");
  Serial.println("Use serial input keys 0 to turn off all debug options at once");
  Serial.println("Use serial input keys QWER to emulate buttons 0 through 3");
  
  for (int o=1; o<10; o++) {    //we don't use zero
    {
      if (debugOptionsText[o]) //don't print undefined options
        Serial.printf("   Option %d = %s %s\n", o, debugOptions[o]?"ON: ":"OFF:", debugOptionsText[o]);
    }
  }
  Serial.println("\n");       //a little extra padding in the output
}


 
