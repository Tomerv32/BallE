/*
//////////////////\\\\\\\\\\\\\\\\\\
//////////////// ** \\\\\\\\\\\\\\\\
/////////////// **** \\\\\\\\\\\\\\\
/\/\/\/\/\/\/ BALL - E \/\/\/\/\/\/\
\\\\\\\\\\\\\\ REMOTE //////////////
\\\\\\\\\\\\\\\\ ** ////////////////
\\\\\\\\\\\\\\\\\\//////////////////
*/

/// ****** LIBRARIES ***** ///
#include <FastLED.h>        // For "EVERY_N_MILLISECONDS()"
//////////////////////////////

/// ****** Gamedpad ****** ///
const int bA = 2;           // "UP" - Boost
const int bB = 3;           // "RIGHT"
const int bC = 4;           // "DOWN" - LEDS
const int bD = 5;           // "LEFT"
const int bK = 8;           // "Joystick Button" - Self Destruct
#define AX    A0            // "X axis"
#define AY    A1            // "Y axis"
//////////////////////////////

/// *** Joystick Values ** ///
int defY = 0;               // Joystick Y value
int deltaY = 0;             // Delta from (0,0) on Y axis
const int Half = 513;       // "Y = 0"
const int Max = 130;        // Max speed
const int Min = 60;         // Min speed
const int err = 30;         // Error correction
//////////////////////////////

/// **** Motor Values **** ///
const int Forward = 10;     //
const int Backward = 1;     //
uint8_t dirc = 0;           //
//////////////////////////////

/// ** Driving Constants * ///
const int SpinSpeed = 130;  // In-place spin speed
const float SlowRate = 0.85;// Slowing down multiplier
const float LowP = 0.4;     // Turning multiplier
const float Boost = 1.5;    // Boost multiplier
const float ErrR = 1.45;    // Fix Right motor slower then Left
//////////////////////////////

/// ** L298N (BT) Values * ///
uint8_t RM = 0;             // Right Motor direction
float RMs = 0;              // Right Motor speed
uint8_t LM = 0;             // Right Motor direction
float LMs = 0;              // Left Motor speed
uint8_t RMsf = 0;           // (int) LMs
uint8_t LMsf = 0;           // (int) RMs
//////////////////////////////

/// **** More Values ***** ///
bool TurnSign = 0;          // Encoders ON/OFF
uint8_t Clicks = 0;         // Clicks on LED Button
bool Pressed = 0;           // LED Button
unsigned long DesTimer = 0; // for millis();
int DesTime = 5000;         // Time to press for Self Destruct
const int LEDTime = 2000;   // Min LED State Time
const int SendTime = 100;   // Sending BT Values delay
//////////////////////////////

/// ************* Functions ************ ///
void GoForward();                         // Set RM & LM to Forward
void GoBackward();                        // Set RM & LM to Backward
void MultipleSpeedsBy(float Multiplier);  // Multiple Speed (For Boost/Turns)
void SetSpeedTo(int Value);               // Set Speed (For In-Place Spin)
void BluetoothSend(char Case, int Value); // Send Values by Bluetooth
void Direction();                         // dirc = Direction Forward/Backward (Both Motors)
void ClearValues();                       // Clear Values (Waiting for User Input)s
////////////////////////////////////////////

void setup() {
  Serial.begin(9600);       // Bluetooth

  pinMode(bA, INPUT_PULLUP); // Buttons
  pinMode(bB, INPUT_PULLUP);
  pinMode(bC, INPUT_PULLUP);
  pinMode(bD, INPUT_PULLUP);
  pinMode(bK, INPUT_PULLUP);
}

void loop() {

  EVERY_N_MILLISECONDS( SendTime )         // Send Values every 100ms
  {
    defY = analogRead(AY);                // Joystick Y axis
    deltaY = abs(defY - Half);
    if (deltaY > err)                     // Joystick moved by user
    {
      RMs = map(deltaY, 0, Half, err, Max); // Mapping to Max Speed
      LMs = RMs;

      if (defY > Half + err)              // Forward
        GoForward();
      else if (defY < Half - err)         // Backward
        GoBackward();
    }

    else if (RMs > err && LMs > err)      // Slowing down, same direction
    {
      if (dirc == Forward)              // Keep sending direction for slowing down!
        GoForward();
      else if (dirc == Backward)
        GoBackward();

      MultipleSpeedsBy (SlowRate);
    }


    if (digitalRead(bA) == LOW)           // Send BOOST
      MultipleSpeedsBy (Boost);


    if (digitalRead(bB) == LOW)           // Send Right adjusment
    {
      if (deltaY > Min)                   // Turn Right // >err
      {
        //        LMs = LMs;
        RMs *= LowP * ErrR;
      }
      else                                // In-Place Right Turn
      {
        RM = Backward;
        LM = Forward;
        SetSpeedTo (SpinSpeed);
      }

      TurnSign = 1;
    }
    

    if (digitalRead(bD) == LOW)           // Send Left adjusment
    {
      if (deltaY > Min)                   // Turn Left // RMs/LMs > Min
      {
        RMs *= ErrR;
        LMs *= LowP;
      }
      else                                // In-Place Left Turn
      {
        RM = Forward;
        LM = Backward;
        SetSpeedTo (SpinSpeed);
      }

      TurnSign = 1;
    }
    

    if (digitalRead(bK) == HIGH)          // While not pressed 
      DesTimer = millis();
      
    if (millis() - DesTimer >= DesTime)   // Button pressed > DestructTime
      BluetoothSend ('F', 0);

    
    // Turning ON/OFF LEDs
    if (digitalRead(bC) == LOW)           // LEDS Button
      Pressed = 1;

    if (digitalRead(bC) == HIGH && Pressed) // Count Clicks within LEDTime[ms]
    {
      Pressed = 0;
      Clicks++;
    }

    EVERY_N_MILLISECONDS( LEDTime )       // Send amount of Clicks
    {
      BluetoothSend ('E', Clicks);
      Clicks = 0;
    }
    

    // Max & Min Speed Values
    (RMs > Max) ? RMs = Max : RMs;
    (LMs > Max) ? LMs = Max : RMs;
    (RMs < Min && deltaY > err) ? RMs = Min : RMs;
    (LMs < Min && deltaY > err) ? LMs = Min : LMs;
    (RMs < err) ? RMs = 0 : RMs;
    (LMs < err) ? LMs = 0 : LMs;
    
    // Rounding Speed Values
    RMsf = (int) RMs;
    LMsf = (int) LMs;

    // Sending Values by Bluetooth
    BluetoothSend ('Z', TurnSign);
    BluetoothSend ('A', RM);
    BluetoothSend ('B', RMs);
    BluetoothSend ('C', LM);
    BluetoothSend ('D', LMs);

    // Saving last direction
    Direction();

    // Clearing Values, !Clearing RMsf & LMsf (for Slowing down)
    ClearValues();
  }

}
