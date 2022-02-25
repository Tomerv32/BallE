// ** Folder 1 ** // ** Ball-E ** //
// 001 - Ball-E
// 002 - Ball-E LOW
// 003 - Ball-E LOWEST
// 004 - Ball-E HIGH

// ** Folder 2 ** // ** Car ** //
// 001 - Car Start
// 002 - Car Brake
// 003 - Car Crash
// 004 - Car Alarm SHORT
// 005 - Car Alarm
// 006 - Car Reverse
// 007 - Car Reverse 1 Sec

// ** Folder 3 ** // ** Destruct ** //
// 001 - 20 Seconds to Destruct
// 002 - 10 Seconds to Destruct
// 003 - Destruct (Explode)
// 004 - Beep

// ** Folder 4 ** // ** Extra ** //
// 001 - Wow
// 002 - Firing
// 003 - Computer Talking
// 004 - Computer Talking 4 Sec
// 005 - Computer Talking 3 Sec

// ** Folder 5 ** // ** Numbers ** //
// 1-20 (step = 1), 30-100 (step = 10)
// 101 - Zero
// 102 - Point
// 103 - Announcement
// 104 - Now the time is
// 105 - O'clock
// 106 - And
// 107 - Minus

void ExplodingTime(int Time)
{
  for (int i = 0; i < Time / 200; i++)
  {
    FastLED.setBrightness(random(0, 15)*10);  // "Crazy" LEDS
    FastLED.show();
    EVERY_N_MILLISECONDS (300)                // "Crazy" Front LED
    {
      FrontLEDState = !FrontLEDState;
      digitalWrite(FrontLED, FrontLEDState);
    }
    delay(200);                               // delay for 10..9..8.. count
  }
}


void ExplodeInTen()
{
  myDFPlayer.playFolder(3, 2);                // 10 Sec
  ExplodingTime(2200);

  for (int i = 9; i > 0; i--)                 // 9-1 Sec
  {
    myDFPlayer.playFolder(5, i);
    ExplodingTime(1000);
  }

  myDFPlayer.playFolder(3, 3);                // Explode
  ExplodingTime(2400);

  FastLED.setBrightness(0);                   // Turn OFF everything
  FastLED.show();
  FrontLEDState = 0;
  digitalWrite(FrontLED, FrontLEDState);
  delay(5000);

  myDFPlayer.playFolder(3, 4);                // Beep
  delay(7000);

  myDFPlayer.pause();                         // Nothing
  delay(1500);

  resetFunc();                                // Reset Ball-E!
}
