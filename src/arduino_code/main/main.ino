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

#define NUM_FEET_STRIPS 2
#define NUM_FEET_LEDS 42


// LEDS STRIP PINS
#define STRIP1 2
#define STRIP2 3
#define STRIP3 4
#define STRIP4 5
#define STRIP5 6
#define STRIP6 7

#define FEET_STRIP1 8
#define FEET_STRIP2 9

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int lightThresholdOne = 2;
int lightThresholdTwo = 2;

int sensorPinOne = A0;
int sensorPinTwo = A1;

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

    FastLED.addLeds<NEOPIXEL, FEET_STRIP1>(feets[0], NUM_FEET_LEDS);
    FastLED.addLeds<NEOPIXEL, FEET_STRIP2>(feets[1], NUM_FEET_LEDS);

    randomSeed(analogRead(5));
    debug_print("setup done");
}


void loop() 
{
    uint8_t sensor_values[2];
    read_sensors(sensor_values);
    control_feet(sensor_values);
    newSensorValue = decide_value(sensor_values);
    
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
  fadeToBlackBy(feets[strip], NUM_FEET_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_FEET_LEDS-1 );
  feets[strip][pos] += CHSV( gHue, 255, 192);
  gHue++;
}

void inactive_foot(uint8_t strip)
{
  fadeToBlackBy(feets[strip], NUM_FEET_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_FEET_LEDS-1 );
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


uint8_t read_sensor_one()
{
  int sensorValue = analogRead(sensorPinOne);
  debug_print(sensorValue);

  if(sensorValue < lightThresholdOne)
  {
    return 1;  
  }
  return 0;  
}


uint8_t read_sensor_two()
{

   int sensorValue = analogRead(sensorPinTwo);
   debug_print(sensorValue);

  if(sensorValue < lightThresholdTwo)
  {
    return 1;  
  }
  return 0;
}




void read_sensors(uint8_t sensor_values[])
{
  
  uint8_t value_one = read_sensor_one();
  uint8_t value_two = read_sensor_two();

  sensor_values[0] = value_one;
  sensor_values[1] = value_two;

}

uint8_t decide_value(uint8_t values[])
{
  uint8_t value_one = values[0];
  uint8_t value_two = values[1];
  if(value_one == 1 && value_two == 1)
  {
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
