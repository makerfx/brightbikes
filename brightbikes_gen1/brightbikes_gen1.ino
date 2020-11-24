//todo: I think I'm sending data to invalid pixels based on the pixel mapping values - should I change all of them 
//      that are invalid pixels to the same address - basically set numleds + 1 and map all extras to the safety pixel

//todo: animated gifs - https://github.com/pixelmatix/AnimatedGIFs/blob/master/examples/SmartMatrixGifPlayer/SmartMatrixGifPlayer.ino

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


bool debugOptions[10] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0};   //change default here, helpful for startup debugging

uint8_t aniMode = 0;
                                                        
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

#define LED_PIN 24


// Params for width and height
const uint8_t kMatrixWidth = 62;
const uint8_t kMatrixHeight = 27;
const uint8_t LED_COLS = kMatrixWidth;
const uint8_t LED_ROWS = kMatrixHeight;


//const uint16_t XYTableFront[] = {0,1,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,2,3,4,5,6,7,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,8,9,10,11,12,13,14,15,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,16,17,18,19,20,21,22,23,24,25,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,26,27,28,29,30,31,32,33,34,35,36,37,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,38,39,40,41,42,43,44,45,46,47,48,49,50,275,276,277,278,279,280,281,282,283,284,285,286,287,288,51,52,53,54,55,56,57,58,59,60,61,62,63,64,289,290,291,292,293,294,295,296,297,298,299,300,301,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,302,303,304,305,306,307,308,309,310,311,312,313,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,314,315,316,317,318,319,320,321,322,323,324,325,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,326,327,328,329,330,331,332,333,334,335,336,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,337,338,339,340,341,342,343,344,345,346,347,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,348,349,350,351,352,353,354,355,356,357,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,358,359,360,361,362,363,364,365,366,367,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,368,369,370,371,372,373,374,375,376,377};
//const uint16_t XYTableMid[]   = {299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,0,1,2,3,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,4,5,6,7,8,9,10,11,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,12,13,14,15,16,17,18,19,20,21,22,23,354,355,356,357,358,359,360,361,362,363,364,365,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,366,367,368,369,370,371,372,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,373,374,375,376,377,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,378,379,380,381,382,383,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,384,385,386,387,388,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,389,390,391,392,393,394,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,395,396,397,398,399,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,400,401,402,403,404,405,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,406,407,408,409,410,411,412,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,413,414,415,416,417,418,419,420,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,421,422,423,424,425,426,427,428,429,430,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,431,432,433,434,435,436,437,438,439,440,441,442,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,261,262,263,264,265,266,267,268,269,270,271,272,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,273,274,275,276,277,278,279,280,281,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,282,283,284,285,286,287,288,496,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,289,290,291,292,293,294,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,295,296,297,298};
//const uint16_t XYTableRear[]  = {0,1,2,3,4,5,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,6,7,8,9,10,11,12,13,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,14,15,16,17,18,19,20,21,22,23,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,24,25,26,27,28,29,30,31,32,33,34,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,35,36,37,38,39,40,41,42,43,44,45,46,47,412,413,414,415,416,417,418,419,420,421,422,423,424,425,48,49,50,51,52,53,54,55,56,57,58,59,60,426,427,428,429,430,431,432,433,434,435,436,437,438,439,61,62,63,64,65,66,67,68,69,70,71,72,73,74,440,441,442,443,444,445,446,447,448,449,450,451,452,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,453,454,455,456,457,458,459,460,461,462,463,464,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,465,466,467,468,469,470,471,472,473,474,475,476,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,477,478,479,480,481,482,483,484,485,486,487,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,488,489,490,491,492,493,494,495,496,497,498,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,499,500,501,502,503,504,505,506,507,508,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,509,510,511,512,513,514,515,516,517,518,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,519,520,521,522,523,524,525,526,527,528,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,529,530,531,532,533,534,535,536,537,538,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,539,540,541,542,543,544,545,546,547,548,549,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,550,551,552,553,554,555,556,557,558,559,560,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,561,562,563,564,565,566,567,568,569,570,571,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,572,573,574,575,576,577,578,579,580,581,582,583,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,584,585,586,587,588,589,590,591,592,593,594,595,283,284,285,286,287,288,289,290,291,292,293,294,295,296,596,597,598,599,600,601,602,603,604,605,606,607,608,297,298,299,300,301,302,303,304,305,306,307,308,309,609,610,611,612,613,614,615,616,617,618,619,620,621,622,310,311,312,313,314,315,316,317,318,319,320,321,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,322,323,324,325,326,327,328,329,330,331,332,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652,653,654,655,656,333,334,335,336,337,338,657,658,659,660,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755};

//reversed
const uint16_t XYTableFront[] = {161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,368,369,370,371,372,373,374,375,376,377,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,358,359,360,361,362,363,364,365,366,367,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,348,349,350,351,352,353,354,355,356,357,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,337,338,339,340,341,342,343,344,345,346,347,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,326,327,328,329,330,331,332,333,334,335,336,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,314,315,316,317,318,319,320,321,322,323,324,325,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,302,303,304,305,306,307,308,309,310,311,312,313,51,52,53,54,55,56,57,58,59,60,61,62,63,64,289,290,291,292,293,294,295,296,297,298,299,300,301,38,39,40,41,42,43,44,45,46,47,48,49,50,275,276,277,278,279,280,281,282,283,284,285,286,287,288,26,27,28,29,30,31,32,33,34,35,36,37,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,16,17,18,19,20,21,22,23,24,25,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,8,9,10,11,12,13,14,15,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,2,3,4,5,6,7,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,0,1,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202};
const uint16_t XYTableMid[]   = {517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,295,296,297,298,496,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,289,290,291,292,293,294,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,282,283,284,285,286,287,288,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,273,274,275,276,277,278,279,280,281,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,261,262,263,264,265,266,267,268,269,270,271,272,431,432,433,434,435,436,437,438,439,440,441,442,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,421,422,423,424,425,426,427,428,429,430,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,413,414,415,416,417,418,419,420,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,406,407,408,409,410,411,412,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,400,401,402,403,404,405,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,395,396,397,398,399,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,390,391,392,393,394,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,385,386,387,388,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,389,380,381,382,383,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,384,375,376,377,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,378,379,368,369,370,371,372,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,373,374,356,357,358,359,360,361,362,363,364,365,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,366,367,341,342,343,344,345,346,347,348,349,350,351,352,353,12,13,14,15,16,17,18,19,20,21,22,23,354,355,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,4,5,6,7,8,9,10,11,338,339,340,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,0,1,2,3,318,319,320,321};
const uint16_t XYTableRear[]  = {729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,654,655,656,333,334,335,336,337,338,657,658,659,660,661,662,663,664,665,666,667,668,669,670,671,672,673,674,322,323,324,325,326,327,328,329,330,331,332,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652,653,310,311,312,313,314,315,316,317,318,319,320,321,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,297,298,299,300,301,302,303,304,305,306,307,308,309,609,610,611,612,613,614,615,616,617,618,619,620,621,622,283,284,285,286,287,288,289,290,291,292,293,294,295,296,596,597,598,599,600,601,602,603,604,605,606,607,608,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,584,585,586,587,588,589,590,591,592,593,594,595,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,572,573,574,575,576,577,578,579,580,581,582,583,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,561,562,563,564,565,566,567,568,569,570,571,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,550,551,552,553,554,555,556,557,558,559,560,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,539,540,541,542,543,544,545,546,547,548,549,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,529,530,531,532,533,534,535,536,537,538,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,519,520,521,522,523,524,525,526,527,528,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,509,510,511,512,513,514,515,516,517,518,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,499,500,501,502,503,504,505,506,507,508,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,488,489,490,491,492,493,494,495,496,497,498,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,477,478,479,480,481,482,483,484,485,486,487,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,465,466,467,468,469,470,471,472,473,474,475,476,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,453,454,455,456,457,458,459,460,461,462,463,464,61,62,63,64,65,66,67,68,69,70,71,72,73,74,440,441,442,443,444,445,446,447,448,449,450,451,452,48,49,50,51,52,53,54,55,56,57,58,59,60,426,427,428,429,430,431,432,433,434,435,436,437,438,439,35,36,37,38,39,40,41,42,43,44,45,46,47,412,413,414,415,416,417,418,419,420,421,422,423,424,425,24,25,26,27,28,29,30,31,32,33,34,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,14,15,16,17,18,19,20,21,22,23,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,6,7,8,9,10,11,12,13,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,0,1,2,3,4,5,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359};


const uint8_t arrayStart[] = {0, 15, 15+20};

#define COLOR_ORDER GRB
#define CHIPSET     WS2812SERIAL

#define BRIGHTNESS 64

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define NUM_LEDS_FRONT (14 * kMatrixHeight)  
#define NUM_LEDS_MID (20 * kMatrixHeight)
#define NUM_LEDS_REAR (28 * kMatrixHeight)

//CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
//CRGB* const leds( leds_plus_safety_pixel + 1);

CRGB leds1[ NUM_LEDS_FRONT + 1];
CRGB leds2[ NUM_LEDS_MID + 1];
CRGB leds3[ NUM_LEDS_REAR + 1];
CRGB*  ledsF( leds1+1);
CRGB*  ledsM( leds2+1 );
CRGB*  ledsR( leds3+1 );

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

 FastLED.addLeds<CHIPSET, LED_PIN_FRONT, COLOR_ORDER>(ledsF, NUM_LEDS_FRONT).setCorrection(TypicalSMD5050);
 FastLED.addLeds<CHIPSET, LED_PIN_MID, COLOR_ORDER>(ledsM, NUM_LEDS_MID).setCorrection(TypicalSMD5050);
 FastLED.addLeds<CHIPSET, LED_PIN_REAR, COLOR_ORDER>(ledsR, NUM_LEDS_REAR).setCorrection(TypicalSMD5050);
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
uint16_t XY( uint8_t a, uint8_t x, uint8_t y)
{
  uint16_t i = (y * kMatrixHeight) + x;
  uint16_t j = 0;
  if (a == 1)  {
    j = XYTableFront[i];
  }
  else if (a==2) {
    j = XYTableMid[i];
  }
  else if (a==3) j = XYTableRear[i];

  return j;
 
}

/*
 * setLEDVal - this function calls XY and splits the array across the panels
 * todo: fix the reversed x & y
 */
void setLEDVal (uint8_t x, uint8_t y, CRGB val) {


/*
        Serial.print(x);
        Serial.print(",");
        Serial.print(y);
        Serial.print(",");
        Serial.print(val.red);
        Serial.print(val.green);
        Serial.println(val.blue);
        
        if (val) 
          {
            display.drawPixel(x, SCREEN_HEIGHT - y, SSD1306_WHITE);
            //display.display();
            //delay(1);
          }
        
*/        

     bool oled = false;
     uint16_t xyout = 0;
     //if ((val.red > 10) && (val.green > 10) & (val.blue >10)) oled = true;
     if (val.red > 10) oled = true;


     if (x < arrayStart[1]) {
        xyout= XY(1, y, x);
        ledsF[xyout]  = val;
        if (xyout <178) {
          if (oled) display.drawPixel(2*x, SCREEN_HEIGHT - y, SSD1306_WHITE);
        }
      }
      else if (x >= arrayStart[1] && x < arrayStart[2]) {
        xyout = XY(2, y, x-arrayStart[1]);
        ledsM[xyout]  = val;
        if (oled && (xyout < 299)) display.drawPixel(2*x+2, SCREEN_HEIGHT - y, SSD1306_WHITE);
      }
      else {
        xyout= XY(3, y, x - arrayStart[2]);
        ledsR[xyout]  = val;
        if (oled && (xyout <339)) display.drawPixel(2*x+4, SCREEN_HEIGHT - y, SSD1306_WHITE);
      }
      
}


int testX = 0; 
int testY = 0;

void loop()
{

   
   switch (aniMode) {
    case 0: aniFire(); break;
    case 1: aniSinPlasma(1); break;
    case 2: aniSinPlasma(2); break;
    case 3: aniSinPlasma(3); break;
    case 4: aniSinPlasma(4); break;
    case 5: aniXYMatrixHueShift(); break;
    case 6: aniTestCrawl(); break;
    case 7: aniTestSweep(); break;
    default: aniMode = 0;
   }
    

  display.setCursor(0,0);
  display.print("FPS:");
  display.print(FastLED.getFPS());
  
  display.display();
  FastLED.show();
  display.clearDisplay();

  
  debugOptionsCheck();   
  
}

void aniTestCrawl() {
    //Serial.print(testX);
    //Serial.print(",");
    //Serial.println(testY);
    //Serial.print(":");
    // Serial.println(XY(y,x));

    CRGB val = ColorFromPalette (RainbowColors_p,(millis()/20)%255,255);
    setLEDVal(testX,testY, val);
    
    //setLEDVal(testX,testY, CHSV(100, 255, 255));
    
    testY = testY+1; 
    if (testY> kMatrixHeight-1) {
      testY=0;
      testX +=1;
    }
    
    if (testX> kMatrixWidth) {
      testX = 0;
      testY=0;
    }

}

void aniTestSweep() {
  //horizontal sweep left to right
  for (int i=0; i<=LED_COLS; i++) {
    for (int j=0; j<LED_ROWS; j++) {
      setLEDVal(i, j, ColorFromPalette (RainbowColors_p,(millis()/10)%255,255));
    }
    FastLED.show();
    debugOptionsCheck(); 
    FastLED.clear();
  }

  //horizontal sweep right to left
  for (int i=LED_COLS; i>=0; i--) {
    for (int j=0; j<LED_ROWS; j++) {
      setLEDVal(i, j, ColorFromPalette (RainbowColors_p,(millis()/10)%255,255));     
    }
    FastLED.show();
    debugOptionsCheck(); 
    FastLED.clear();
  }

  //vertical sweep bottom to top
  for (int i=0; i<=LED_ROWS; i++) {
    for (int j=0; j<LED_COLS; j++) {
      setLEDVal(j, i, ColorFromPalette (RainbowColors_p,(millis()/5)%255,255));
    }
    FastLED.show();
    debugOptionsCheck();
    delay(20); 
    FastLED.clear();
  }

 //vertical sweep top to bottom
  for (int i=LED_ROWS; i>=0; i--) {
    for (int j=0; j<LED_COLS; j++) {
      setLEDVal(j, i, ColorFromPalette (RainbowColors_p,(millis()/5)%255,255));
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
     setLEDVal (i, j, val);
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
      setLEDVal((LED_COLS - 1) - i, (LED_ROWS - 1) - j, ColorFromPalette(myPal, qsub8(inoise8(i * scalexy, j * scalexy + a, a / speed), abs8(j - (LED_ROWS - 1)) * 255 / (LED_ROWS - 1)), BRIGHTNESS));
    }
  }
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
      Serial.print(x);
      Serial.print(",");
      Serial.println(y);
      setLEDVal(x, y, CHSV( pixelHue, 255, 255));
     
    }
  }
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


 
