#include <FastLED.h>

#define BAUDRATE 9600
#define NUM_STRIPS 1
#define NUM_LEDS 50

#define STRIP1 2


int lightThreshold = 70;
int sensorPin = A0;

int one_high = 11;
int one_low = 10;

CRGB leds[NUM_STRIPS][NUM_LEDS];

uint8_t gHue = 0;

void setup() 
{
    SerialUSB.begin(BAUDRATE);
    pinMode(sensorPin, OUTPUT);
    FastLED.addLeds<NEOPIXEL, STRIP1>(leds[0], NUM_LEDS);
}


void loop() 
{
  int sensorValue = analogRead(sensorPin);
  
  if(sensorValue < lightThreshold)
  {
    SerialUSB.println(one_high);
    sinelon();
    FastLED.show(); 
  }
  else
  {
    SerialUSB.println(one_low);
    turn_off();
  }
   FastLED.show();
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds[0], NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[0][pos] += CHSV( gHue, 255, 192);
  gHue++;
}

void turn_off()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    for(int j = 0; j < NUM_STRIPS; j++)
    {
      Serial.println(i);
      Serial.println(j);
      leds[j][i] = CRGB::Black;
    }
  }
}
