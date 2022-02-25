/*
//////////////////\\\\\\\\\\\\\\\\\\
//////////////// ** \\\\\\\\\\\\\\\\
/////////////// **** \\\\\\\\\\\\\\\
/\/\/\/\/\/\/ BALL - E \/\/\/\/\/\/\
\\\\\\\\\\\\\\\ BODY ///////////////
\\\\\\\\\\\\\\\\ ** ////////////////
\\\\\\\\\\\\\\\\\\//////////////////
*/

/// ********* LIBRARIES ******** ///
#include "FastLED.h"              // RGB
                                  //
#include "Arduino.h"              // MP3
#include "SoftwareSerial.h"       // MP3
#include "DFRobotDFPlayerMini.h"  // MP3
////////////////////////////////////

/// ******* USER SETTINGS ****** ///
const int BRIGHTNESS = 50;  //180     // RGB MAX Brightness
const int BrightnessLevels = 3;   // RGB Brightness Levels
                                  //
const int Volume = 29;            // Speaker Volume
                                  //
const int encoderC = 21;          // Encoders - Amount to fix
////////////////////////////////////

/// ********* RGB & LED ******** ///
uint8_t BrightnessState = 2;      // Default Brightness Level
                                  //
// Top Ring LED                   //
const int RingLED = 11;           // Pin (PWM)
const int RNUM_LEDS = 24;         // Ring LED amount
CRGB Rleds[RNUM_LEDS];            //
                                  //
// Bottom LED                     //
const int BottomLED = 4;          // Pin (PWM)
const int BNUM_LEDS = 23;         // Bottom LED amount
CRGB Bleds[BNUM_LEDS];            //
                                  //
// Animation - RGB                //
int gHue = 0;                     // RGB Color
int animSpeed = 0;                // Rainbow speed
                                  //
// Front LED                      //
#define FrontLED A0               // Pin
bool FrontLEDState = 0;           // Default - OFF
////////////////////////////////////

/// ************ MP3 *********** ///
SoftwareSerial MP3Serial(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;   //
bool Start = 0;                   // Flag for "Car Start" Sound
////////////////////////////////////

/// ********** Motors ********** ///
const int EN_A = 5;               // Right Motor Pin (PWM)
const int IN1 = 6;                // R
const int IN2 = 7;                // R
const int IN3 = 8;                // L
const int IN4 = 9;                // L
const int EN_B = 10;              // Left Motor Pin (PWM)
////////////////////////////////////

/// ********* Encoders ********* ///
const int encoderR = 3;           // Pin (Interrupt)
const int encoderL = 2;           // Pin (Interrupt)
volatile int SumL = 0;            // For Interrupt Sum
volatile int SumR = 0;            //
////////////////////////////////////

/// **** Bluetooth Info (IN) *** ///
String datas;                     // Store Serial data
uint8_t infonum = 0;              // (int) of Serial data
uint8_t RM = 0;                   // Right Motor direction
uint8_t LM = 0;                   // Left Motor direction
uint8_t RMs = 0;                  // Right Motor speed
uint8_t LMs = 0;                  // Left Motor speed
uint8_t dirc = 0;                 // Both motors 10 || 01
bool TurnSign = 0;                // Flag for Right/Left Buttons
const uint8_t Forward = 10;       // INa, INb Parameters for Forward
const uint8_t Backward = 1;       // INa, INb Parameters for Backward (1=01)
////////////////////////////////////

/// ********* Functions ******** ///
void(* resetFunc) (void) = 0;     // Go To Line 0
void Direction();                 // dirc = Direction Forward/Backward (Both Motors)
void EncoderR();                  // Count Right Encoder "1"s - Interrupt
void EncoderL();                  // Count Left Encoder "1"s - Interrupt
void SpeedFix();                  // Encoders Speed FIX
void ExplodingTime(int Time);     // Counting To Explode, Time [ms]
void ExplodeInTen();              // Explode Sequence
void MyRainbow(CRGB arr[], int N);// Rainbow RGB Effect
void RGBLeds();                   // RGB LEDS Control
void AnimSpeed();                 // RGB Animation Speed by Motors Speed
////////////////////////////////////


void setup() {

  /// ***** MP3 ***** ///
  MP3Serial.begin(9600);
  if (!myDFPlayer.begin(MP3Serial)) {}
  myDFPlayer.volume(Volume);  //
  delay(1000);
  myDFPlayer.playFolder(4, 4);  // "Computer Talking 4 Sec" Sound
  delay(5000);
  myDFPlayer.playFolder(1, 4);  // "Ball-E" Sound
  delay(2000);

  /// ***** RGB & LED ***** ///
  FastLED.addLeds<WS2812, RingLED, GRB>(Rleds, RNUM_LEDS);    // Declare Ring LED
  FastLED.addLeds<WS2812, BottomLED, GRB>(Bleds, BNUM_LEDS);  // Declare Bottom LED
  FastLED.setBrightness(BrightnessState * BRIGHTNESS / BrightnessLevels); // Set LEDs brightness
  pinMode(FrontLED, OUTPUT);
  digitalWrite(FrontLED, FrontLEDState);  // Front LED default

  /// ***** Motors ***** ///
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1, LOW); // All Motor OFF by default
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN_A, 0); // Set initial Motors speed to 0
  analogWrite(EN_B, 0);

  /// ***** Bluetooth ***** ///
  Serial.begin(9600);
  if (Serial.available() > 0)
    datas = Serial.readStringUntil('\n');
  datas = "0";   // Clear first read & mark as "0"

  /// ***** Encoders ***** ///
  pinMode(encoderL, INPUT_PULLUP);
  pinMode(encoderR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderR), EncoderR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderL), EncoderL, CHANGE);
}


void loop() {

  /// ***** RGB ***** ///
  RGBLeds();
    
  if (Serial.available() > 0)
  {
    datas = Serial.readStringUntil('!');
    if (!Start && datas != "0")       // Only after receiving some BT data
    {
      myDFPlayer.playFolder(2, 1);    // "Car Start" Sound
      Start = 1;                      // Flag
    }
  }
  
  String info = "";                   // For the data we need
  if (datas != "0")
  {
    int offset = datas.indexOf("U");  // Header of received data
    if (offset >= 0)
    {
      info = datas.substring(offset + 2, datas.indexOf('!')); // Take the important data
      infonum = info.toInt();                                 // Make it INT
      char value = datas.charAt(offset + 1);                  // Check CASE
      switch (value)
      {
        case 'Z': // TurnSign
          {
            TurnSign = infonum;
            break;
          }

        case 'A': // Right Motor
          {
            (infonum / 10 == 1) ? digitalWrite(IN1, HIGH) : digitalWrite(IN1, LOW);
            (infonum % 10 == 1) ? digitalWrite(IN2, HIGH) : digitalWrite(IN2, LOW);
            RM = infonum;
            break;
          }

        case 'B': // Right Motor Speed
          {
            if (!TurnSign)      // Encoders only when going Forward/Backward
            {
              EVERY_N_MILLISECONDS (100)
              {
                analogWrite(EN_A, infonum);
                RMs = infonum;  // Save Speed
              }
            }
            else
            {
              analogWrite(EN_A, infonum);
              RMs = infonum;    // Save Speed
            }
            break;
          }

        case 'C': // Left Motor
          {
            (infonum / 10 == 1) ? digitalWrite(IN3, HIGH) : digitalWrite(IN3, LOW);
            (infonum % 10 == 1) ? digitalWrite(IN4, HIGH) : digitalWrite(IN4, LOW);
            LM = infonum;       // Save direction
            break;
          }

        case 'D': // Left Motor Speed
          {
            if (!TurnSign)      // Encoders only when going Forward/Backward
            {
              EVERY_N_MILLISECONDS (100)
              {
                analogWrite(EN_B, infonum);
                LMs = infonum;    // Save Speed
              }
            }
            else
            {
              analogWrite(EN_B, infonum);
              LMs = infonum;    // Save Speed
            }
            break;
          }

        case 'E': // LEDS
          {
            if (infonum == 1)       // 1 Click = Front LED
            {
              FrontLEDState = !FrontLEDState;
              digitalWrite(FrontLED, FrontLEDState);
              if (FrontLEDState)
                myDFPlayer.playFolder(4, 2);  // "Firing" Sound
            }

            else if (infonum == 2)  // 2 Clicks = RGB LEDS
            {
              BrightnessState++;    // Next Brightness Level
              (BrightnessState > BrightnessLevels) ? BrightnessState = 0 : BrightnessState;
              FastLED.setBrightness(BrightnessState * BRIGHTNESS / BrightnessLevels);
              FastLED.show();
            }
            
            datas = "0";            // Do Not repeat!
            break;
          }
          
        case 'F': // Self Destruct
          {
            ExplodeInTen();         // Seld Destruct Function
            break;
          }
      }
    }

    Direction();
    
    /// ***** RGB ***** ///
    EVERY_N_MILLIS_I(thisTimer, 0)
    {
      AnimSpeed();
      thisTimer.setPeriod(animSpeed);
    }
    
    /// ***** Encoders ***** ///
    if (!TurnSign && (dirc == Forward || dirc == Backward)) // Encoders - Only when going Forward/Backward
    {
      EVERY_N_MILLISECONDS (25) // Fix Time
      {SpeedFix();}
    }
  
    EVERY_N_MILLISECONDS (100)  // Clear
    {
      SumR = 0;
      SumL = 0;
    }
  
    /// ***** MP3 ***** ///
    if (TurnSign)                       // When In-Place Turn
    {
      EVERY_N_MILLISECONDS (3000)
        {myDFPlayer.playFolder(4, 1);}  // "Wow" Sound
    }
  
    if (dirc == Forward)                // When Forward
    {
      EVERY_N_MILLISECONDS (7000)
        {myDFPlayer.playFolder(4, 5);}  // "Computer Talking 3 Sec" Sound
    }
      
    if (dirc == Backward)               // When Backward
    {
      EVERY_N_MILLISECONDS (1000)
        {myDFPlayer.playFolder(2, 7);}  // "Car Reverse" Sound
    }
    
  }
}
