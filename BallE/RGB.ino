void MyRainbow(CRGB arr[], int N)
{
  for (int i = 0; i < N; i++)
    arr[i].setHue(gHue + 11 * i);       // Set all LEDS
}


void RGBLeds()
{
  MyRainbow(Rleds, RNUM_LEDS);          // Top Ring rainbow

  // Bottom LED White/Red (Forward/Backward) | Rainbow
  if (dirc == Forward)
    fill_solid(Bleds, BNUM_LEDS, CRGB::White);
  else if (dirc == Backward)
    fill_solid(Bleds, BNUM_LEDS, CRGB::Red);
  else
    MyRainbow(Bleds, BNUM_LEDS);

  FastLED.show();
}


void AnimSpeed()
{
  animSpeed = (130 -  max(RMs, LMs)) / 2;
  if (RM != 0 || LM != 0)               // Only when Ball-E is actually moving
  {
    if (animSpeed < 0)
      animSpeed = 0;
    else if (animSpeed > 65)
      animSpeed = 65;
  }
  (dirc == Backward) ? gHue-- : gHue++; // Clockwise/Counterclockwise direction
}
