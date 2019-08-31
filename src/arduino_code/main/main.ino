#include <arduino.h>
#include <FastLED.h>

#define SENSOR_ON

// #define DEBUG

#ifdef DEBUG
 #define debug_print(x)  Serial.println (x)
#else
 #define debug_print(x)
#endif

#define BAUDRATE 250000

#define NUM_STRIPS 6
#define NUM_LEDS 60

#define NUM_FEET_STRIPS 1
#define NUM_FEET_LEDS 42


// LEDS STRIP PINS
#define STRIP1 2
#define STRIP2 3
#define STRIP3 4
#define STRIP4 5
#define STRIP5 6
#define STRIP6 7

#define FEET_STRIP1 8

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int lightThresholdOne = 40;
int lightThresholdTwo = 40;
int sensorPinOne = A0;
int sensorPinTwo = A1;

int one_high = 1;
int one_low = 0;
int two_high = 21;
int two_low = 20;

CRGB leds[NUM_STRIPS][NUM_LEDS];
CRGB feets[NUM_FEET_STRIPS][NUM_FEET_LEDS]

uint8_t gHue = 0;
bool newSensorValue;
bool previousSensorValue = false;

bool boots = false;

void sinelon_reverse();
void sinelon();
void juggle();


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList patterns = { sinelon_reverse, sinelon, juggle };
uint8_t currentPattern = 0;

void setup() 
{
    Serial.begin(BAUDRATE);
    SerialUSB.begin(BAUDRATE);

    debug_print("Setup...");
    
    pinMode(sensorPinOne, INPUT);
    pinMode(sensorPinTwo, INPUT);
    
    FastLED.addLeds<NEOPIXEL, STRIP1>(leds[0], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP2>(leds[1], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP3>(leds[2], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP4>(leds[3], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP5>(leds[4], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP6>(leds[5], NUM_LEDS);

    FastLED.addLeds<NEOPIXEL, FEET_STRIP1>(feets[0], NUM_LEDS);

    randomSeed(analogRead(4));
}


void loop() 
{

    newSensorValue = read_sensors();
    
    if(newSensorValue == true && previousSensorValue == false)
    {
        boots = true;
        send_to_pi(one_high);
    }
    else if(newSensorValue == false && previousSensorValue == true)
    {
        boots = false;
        send_to_pi(one_low);
    }

    if(boots == true){
      ligths_on();
    }
    else
    {
      turn_off();
    }

    previousSensorValue = newSensorValue;    
    FastLED.show();

    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

}

void ligths_on()
{
  patterns[currentPattern]();

}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  currentPattern = (currentPattern + 1) % ARRAY_SIZE( patterns);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
    fadeAllStripsToBlackBy(20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );

    for(int i=0; i<NUM_STRIPS; i++)
    { 
        int pos = beatsin16( 13, 0, NUM_LEDS-1 );
        leds[i][pos] += CHSV( gHue, 255, 192);
        gHue++;
    }
}

void sinelon_reverse()
{
  // a colored dot sweeping back and forth, with fading trails
    fadeAllStripsToBlackBy(20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );

    for(int i=0; i<NUM_STRIPS; i++)
    { 
        if(i % 2 == 0)
        {
          int pos = beatsin16( 13, 0, NUM_LEDS-1 );
          leds[i][pos] += CHSV( gHue, 255, 192);
          gHue++;
        }
        else
        {
          int pos = beatsin16( 13, 0, NUM_LEDS -1);
          leds[i][NUM_LEDS - pos] += CHSV( gHue, 255, 192);
          gHue++;
        }
        
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeAllStripsToBlackBy(20);
    byte dothue = 0;

    for( int i = 0; i < 8; i++) {
        for(int j=0; j<NUM_STRIPS; j++)
        {
          leds[j][beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
          dothue += 32;
        }

    }
}


int read_sensor_one()
{
  #ifdef SENSOR_ON
    int sensorValue = analogRead(sensorPinOne);
  #else
    int sensorValue = lightThresholdOne - 1;
  #endif

  if(sensorValue < lightThresholdOne)
  {
    return true;  
  }
  return false;  
}



int read_sensor_two()
{
  #ifdef SENSOR_ON
    int sensorValue = analogRead(sensorPinTwo);
  #else
    int sensorValue = lightThresholdTwo - 1;
    debug_print(sensorValue);
  #endif

  if(sensorValue < lightThresholdTwo)
  {
    return true;  
  }
  return false;
}




int read_sensors()
{
  
  bool value_one = read_sensor_one();
  bool value_two = read_sensor_two();

  if(value_one == true && value_two == true)
  {
    return true;
  }
  return false;
}





void fadeAllStripsToBlackBy(int value)
{
    for(int i=0; i<NUM_STRIPS; i++)
    { 
        fadeToBlackBy( leds[i], NUM_LEDS, value);
    }
}

void send_to_pi(int value)
{
  SerialUSB.println(value);
}

void turn_off()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    for(int j = 0; j < NUM_STRIPS; j++)
    {
      leds[j][i] = CRGB::Black;
    }
  }
}
