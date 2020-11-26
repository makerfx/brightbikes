
//todo: animated gifs - https://github.com/pixelmatix/AnimatedGIFs/blob/master/examples/SmartMatrixGifPlayer/SmartMatrixGifPlayer.ino
//todo: gfx library - look here for info: https://github.com/marcmerlin/Framebuffer_GFX + https://github.com/marcmerlin/FastLED_NeoMatrix

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

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

//oled screen includes
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_PIN_REAR  29
#define LED_PIN_MID  24
#define LED_PIN_FRONT  17

// Params for width and height
const uint8_t kMatrixWidth = 62;
const uint8_t kMatrixHeight = 27;
const uint8_t LED_COLS = kMatrixWidth;
const uint8_t LED_ROWS = kMatrixHeight;

//all in one
const int XYTable[] = {161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,51,52,53,54,55,56,57,58,59,60,61,62,63,64,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,38,39,40,41,42,43,44,45,46,47,48,49,50,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,8,9,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,3,4,5,6,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,473,474,475,476,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,467,468,469,470,471,472,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,460,461,462,463,464,465,466,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,451,452,453,454,455,456,457,458,459,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,439,440,441,442,443,444,445,446,447,448,449,450,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,-1,-1,-1,-1,-1,-1,-1,-1,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,-1,-1,-1,-1,-1,-1,-1,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,-1,-1,-1,-1,-1,-1,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,-1,-1,-1,-1,-1,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,-1,-1,-1,-1,-1,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,-1,-1,-1,-1,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,-1,-1,-1,-1,-1,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,-1,-1,-1,-1,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,-1,-1,-1,-1,-1,-1,-1,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,190,191,192,193,194,195,196,197,198,199,200,201,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,182,183,184,185,186,187,188,189,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,178,179,180,181,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,810,811,812,813,814,815,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,799,800,801,802,803,804,805,806,807,808,809,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,787,788,789,790,791,792,793,794,795,796,797,798,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,774,775,776,777,778,779,780,781,782,783,784,785,786,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,760,761,762,763,764,765,766,767,768,769,770,771,772,773,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,745,746,747,748,749,750,751,752,753,754,755,756,757,758,759,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,648,649,650,651,652,653,654,655,656,657,658,659,660,661,662,663,664,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,614,615,616,617,618,619,620,621,622,623,624,625,626,627,628,629,630,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,598,599,600,601,602,603,604,605,606,607,608,609,610,611,612,613,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,538,539,540,541,542,543,544,545,546,547,548,549,550,551,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,525,526,527,528,529,530,531,532,533,534,535,536,537,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,512,513,514,515,516,517,518,519,520,521,522,523,524,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,501,502,503,504,505,506,507,508,509,510,511,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,491,492,493,494,495,496,497,498,499,500,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,483,484,485,486,487,488,489,490,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,477,478,479,480,481,482,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

#define NUM_LEDS_F 178
#define NUM_LEDS_M 299
#define NUM_LEDS_R 339

const uint8_t  arrayStart[] = {0, 15, 15+20}; //this could be replaced with numPanelCols below
const uint16_t numPanelLEDs[] = {NUM_LEDS_F, NUM_LEDS_M, NUM_LEDS_R};
const uint8_t  numPanelCols[] = {14, 20, 28};

#define COLOR_ORDER GRB
#define CHIPSET     WS2812SERIAL

#define BRIGHTNESS 64


#define MATRIX_SIZE (kMatrixWidth * kMatrixHeight)


//see the FastLED XY example for this "safety pixel" concept
CRGB leds_plus_safety_pixel[ MATRIX_SIZE + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);
CRGB* const ledsM( leds_plus_safety_pixel + 1 + numPanelLEDs[0]);
CRGB* const ledsR( leds_plus_safety_pixel + 1 + numPanelLEDs[0] + numPanelLEDs[1]);


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


 
