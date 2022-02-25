void GoForward()
{
  RM = Forward;
  LM = Forward;
}


void GoBackward()
{
  RM = Backward;
  LM = Backward;
}


void MultipleSpeedsBy(float Multiplier)
{
  RMs *= Multiplier;
  LMs *= Multiplier;
}


void SetSpeedTo(int Value)
{
  RMs = Value;
  LMs = Value;
}


void BluetoothSend(char Case, int Value)
{
    Serial.print("U");        // String Header
    Serial.print(Case);       
    Serial.print(Value);      
    Serial.print('!');        // String End Sign    
}


void Direction()
{
  if (RM == Forward && LM == Forward)
    dirc = Forward;
          
  else if (RM == Backward && LM == Backward)
    dirc = Backward;

  else
    dirc = 0;
}


void ClearValues()
{
    RM = 0;
    LM = 0;
    TurnSign = 0;
}
