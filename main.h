//necessary libraries
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

#define PIN 6

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];
//set number of leds
#define NUM_LEDS 12
//set brightness
#define BRIGHTNESS 220

//setup colour sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//setup neopixels
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 0;     // the number of the pushbutton pin
const int buttonPin1 = 1;     // the number of the pushbutton pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int buttonState1 = 0;

//set blade to OFF
bool bladeOn = false;

//default blade colour (red)
int redBlade = 255;
int blueBlade = 0;
int greenBlade = 0;

void setup() {
  
  //start serial for debug purposes
  Serial.begin(115200);
  
  //initialise sd card
  SD.begin(SDCARD_SS_PIN);
  
  // 44100kHz stereo => 88200 sample rate
  AudioZero.begin(2*44100);

  //play silent wav to prevent horrible buzzing
  File silence = SD.open("silence.wav");
  AudioZero.play(silence);
  
  //set pinmodes for neopixels and make sure they are blank
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin1, INPUT);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //initialise colour sensor
  tcs.begin();
  
  // use these three pins to drive an LED
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  //set pin for colour sensor LED and set it to LOW to switch it off
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  
}

void loop() {
  //read status of buttons
  buttonState = digitalRead(buttonPin);
  buttonState1 = digitalRead(buttonPin1);
  
  //int count = 0;
  
  //if the blade is active play the saber hum sound (deactivated as it seemed to prevent the buttons from working)
//  File hum = SD.open("Hum4.wav");
//  File silence = SD.open("silence.wav");
//  if (bladeOn == true) {
//    AudioZero.play(hum);
//  }
//  else if (bladeOn == false) {
//    AudioZero.play(silence);
//  }

  //when activate button is pressed it will activate the leds and play the saber on sound
  //if the blade is already active it will shut off the leds and play the saber off sound
  if (buttonState == HIGH) {
    Serial.print("button0");
    Serial.println();
	//if blade currently off set it to on and play sound and fire up the neopixels with current colours
    if (bladeOn == false) {
      Serial.print(bladeOn);
      Serial.println();
      File silence = SD.open("silence.wav");
      File activate = SD.open("fx4.wav");
      colorWipe(strip.Color(redBlade, greenBlade, blueBlade), 50);
      AudioZero.play(activate);
      bladeOn = !bladeOn;
      AudioZero.play(silence);
      //delay(500);
      }
	//if blade currently on set it to off, play sound and clear neopixels
    else if (bladeOn == true) {
      Serial.print(bladeOn);
      Serial.println();
      File silence = SD.open("silence.wav");
      File deActivate = SD.open("fx5.wav");
      colorWipe(strip.Color(0, 0, 0), 50);
      AudioZero.play(deActivate);
      bladeOn = !bladeOn;
      AudioZero.play(silence);
      //delay(500);    
    }
  }
  
  //if colour detect button pressed, activate the colour sensor and print values to serial port
  if (buttonState1 == HIGH) {
    colorDetect();
    Serial.print("button1");
    Serial.println();
    Serial.print(redBlade);
    Serial.println();
    Serial.print(greenBlade);
    Serial.println();
    Serial.print(blueBlade);
    Serial.println();
    delay(2000);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  Serial.print(c);
  Serial.println();
  for(uint16_t i=0; i<strip.numPixels(); i++) {

    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorDetect() {
    uint16_t clearC, red, green, blue;
	//activate LED
    digitalWrite(7, HIGH);

    delay(60);  // takes 50ms to read
	//deactivate LED after reading
    digitalWrite(7, LOW);
    
    tcs.getRawData(&red, &green, &blue, &clearC);

	// convert raw data
    uint32_t sum = clearC;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    
	//set the blade to the colours sensed
    redBlade = r;
    blueBlade = g;
    greenBlade = b;

}


uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}
