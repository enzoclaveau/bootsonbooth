#include <arduino.h>
#include <FastLED.h>

#define SENSOR_ON

#define DEBUG

#ifdef DEBUG
 #define debug_print(x)  Serial.println (x)
#else
 #define debug_print(x)
#endif

#define BAUDRATE 250000

#define NUM_STRIPS 6
#define NUM_LEDS 60

#define NUM_FEET_STRIPS 4
#define NUM_FEET_LEDS 42

#define NUM_SENSORS 4

// LEDS STRIP PINS
#define STRIP1 2
#define STRIP2 3
#define STRIP3 4
#define STRIP4 5
#define STRIP5 6
#define STRIP6 7

#define FEET_STRIP1 8
#define FEET_STRIP2 9
#define FEET_STRIP3 10
#define FEET_STRIP4 11

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int lightThresholdOne = 2;
int lightThresholdTwo = 2;
int lightThresholdThree = 2;
int lightThresholdFour = 2;

int sensorPinOne = A0;
int sensorPinTwo = A1;
int sensorPinThree = A2;
int sensorPinFour = A3;

int one_high = 1;
int one_low = 0;
int two_high = 21;
int two_low = 20;

CRGB leds[NUM_STRIPS][NUM_LEDS];
CRGB feets[NUM_FEET_STRIPS][NUM_FEET_LEDS];

uint8_t gHue = 0;
bool newSensorValue;
bool previousSensorValue = false;

bool boots = false;

void sinelon_reverse();
void sinelon();
void juggle();
void continuous();
void continous_reverse();

#define SAW_TOOTH_SPEED 9
#define INACTIVE_SAW_TOOTH_SPEED 6
#define FADE_TO_BLACK_SPEED 20

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList patterns = { continuous, continous_reverse,sinelon_reverse, sinelon, juggle };
uint8_t currentPattern = 0;

void setup() 
{
    Serial.begin(BAUDRATE);
    SerialUSB.begin(BAUDRATE);

    debug_print("Setup...");
    
    pinMode(sensorPinOne, INPUT);
    pinMode(sensorPinTwo, INPUT);
    pinMode(sensorPinThree, INPUT);
    pinMode(sensorPinFour, INPUT);
    
    FastLED.addLeds<NEOPIXEL, STRIP1>(leds[0], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP2>(leds[1], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP3>(leds[2], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP4>(leds[3], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP5>(leds[4], NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, STRIP6>(leds[5], NUM_LEDS);
 
    FastLED.addLeds<NEOPIXEL, FEET_STRIP1>(feets[0], NUM_FEET_LEDS);
    FastLED.addLeds<NEOPIXEL, FEET_STRIP2>(feets[1], NUM_FEET_LEDS);
    FastLED.addLeds<NEOPIXEL, FEET_STRIP3>(feets[2], NUM_FEET_LEDS);
    FastLED.addLeds<NEOPIXEL, FEET_STRIP4>(feets[3], NUM_FEET_LEDS);

    randomSeed(analogRead(5));
    debug_print("setup done");
}


void loop() 
{
    uint8_t sensor_values[NUM_SENSORS];
    read_sensors(sensor_values);
    control_feet(sensor_values);
    newSensorValue = decide_value(sensor_values);
    
    if(newSensorValue == true && previousSensorValue == false)
    { 
        debug_print("BOOTS");
        boots = true;
        send_to_pi(one_high);
    }
    else if(newSensorValue == false && previousSensorValue == true)
    {
        debug_print("REMOVED");
        boots = false;
        send_to_pi(0);
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

void control_feet(uint8_t sensor_values[])
{
  for(int i=0; i < NUM_FEET_STRIPS; i++)
  {
    if(sensor_values[i] == 1)
    {
      active_foot(i);
    }
    else
    {
      inactive_foot(i);
    } 
  }  
}

void active_foot(uint8_t strip)
{
  fadeToBlackBy(feets[strip], NUM_FEET_LEDS, FADE_TO_BLACK_SPEED);
  //int pos = beatsin16( 13, 0, NUM_FEET_LEDS-1 );
  int pos = beat8(SAW_TOOTH_SPEED) % NUM_FEET_LEDS;
  feets[strip][pos] += CHSV( gHue, 255, 192);
  gHue++;
}

void inactive_foot(uint8_t strip)
{
  fadeToBlackBy(feets[strip], NUM_FEET_LEDS, FADE_TO_BLACK_SPEED);
  //int pos = beatsin16( 13, 0, NUM_FEET_LEDS-1 );
  int pos = beat8(INACTIVE_SAW_TOOTH_SPEED) % NUM_FEET_LEDS;
  feets[strip][pos] = CRGB::Red;
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
    fadeAllStripsToBlackBy(FADE_TO_BLACK_SPEED);

    for(int i=0; i<NUM_STRIPS; i++)
    { 
        int pos = beatsin16( 13, 0, NUM_LEDS-1 );
        leds[i][pos] += CHSV( gHue, 255, 192);
        gHue++;
    }
}

void continuous()
{
    fadeAllStripsToBlackBy(FADE_TO_BLACK_SPEED);

    for(int i=0; i<NUM_STRIPS; i++)
    { 
        int pos = beat8(SAW_TOOTH_SPEED) % NUM_LEDS;
        leds[i][pos] += CHSV( gHue, 255, 192);
        gHue++;
    }
}

void continous_reverse()
{
    fadeAllStripsToBlackBy(FADE_TO_BLACK_SPEED);
  for(int i=0; i<NUM_STRIPS; i++)
      { 
          if(i % 2 == 0)
          {
            int pos = beat8(SAW_TOOTH_SPEED) % NUM_LEDS;
            leds[i][pos] += CHSV( gHue, 255, 192);
            gHue++;
          }
          else
          {
            int pos = beat8(SAW_TOOTH_SPEED) % NUM_LEDS;
            leds[i][NUM_LEDS - pos] += CHSV( gHue, 255, 192);
            gHue++;
          }
      }
}

void sinelon_reverse()
{
  // a colored dot sweeping back and forth, with fading trails
    fadeAllStripsToBlackBy(FADE_TO_BLACK_SPEED);

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
    fadeAllStripsToBlackBy(FADE_TO_BLACK_SPEED);
    byte dothue = 0;

    for( int i = 0; i < 8; i++) {
        for(int j=0; j<NUM_STRIPS; j++)
        {
          leds[j][beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
          dothue += 32;
        }

    }
}


uint8_t read_sensor_one()
{
  int sensorValue = analogRead(sensorPinOne);

  if(sensorValue < lightThresholdOne)
  {
    return 1;  
  }
  return 0;  
}

uint8_t read_sensor_two()
{

   int sensorValue = analogRead(sensorPinTwo);

  if(sensorValue < lightThresholdTwo)
  {
    return 1;  
  }
  return 0;
}

uint8_t read_sensor_three()
{

   int sensorValue = analogRead(sensorPinThree);

  if(sensorValue < lightThresholdThree)
  {
    return 1;  
  }
  return 0;
}

uint8_t read_sensor_four()
{

   int sensorValue = analogRead(sensorPinFour);

  if(sensorValue < lightThresholdFour)
  {
    return 1;  
  }
  return 0;
}


void read_sensors(uint8_t sensor_values[])
{
  sensor_values[0] = read_sensor_one();
  sensor_values[1] = read_sensor_two();
  sensor_values[2] = read_sensor_three();
  sensor_values[3] = read_sensor_four();
}

uint8_t decide_value(uint8_t values[])
{
  if(values[0] == 1 && values[1] == 1 && values[2] == 1 && values[3] == 1)
  {
    debug_print("all on");
    return 1;
  }
  return 0;
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
