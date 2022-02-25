void Direction()
{
  if (LM == 10 && RM == 10)
    dirc = Forward;
  else if (LM == 1 && RM == 1)
    dirc = Backward;
  else
    dirc = 0;
}


void EncoderR()
{
  SumR++;
}


void EncoderL()
{
  SumL++;
}


void SpeedFix()
{
  RMs += (SumL > SumR) ? encoderC : 0;  // Fix!
  (RMs > 255) ? RMs = 255 : RMs;        // 255 is MAX

  analogWrite(EN_A, RMs);
}
