void lcdInit()
{
  tft.fillRect(10, 10, 220, 50, RED);
  tft.drawRect(10, 10, 220, 50, WHITE);

  tft.fillRect(10, 70, 220, 50, GREEN);
  tft.drawRect(10, 70, 220, 50, WHITE);

  tft.fillRect(10, 190, 220, 50, BLUE);
  tft.drawRect(10, 190, 220, 50, WHITE);



  tft.fillRect(10, 130, 220, 50, MAGENTA);
  tft.drawRect(10, 130, 220, 50, WHITE);

  
}

void lcdsetup()
{
  setPointValue(0);
  setCounter(0);
  setMrnQuantity(0);
  setRemainingQuantity(0);
  setAuth("AUTHENTIC");
}





void setPointValue(float value)
{
  if( value != previous_setpoint)
  {
    previous_setpoint = value;
    tft.setCursor(15, 20);
    tft.setTextColor(WHITE, RED);
    tft.setTextSize(2);
    tft.print("       ");
    tft.setCursor(15, 20);
    tft.print("SET POINT " + String(value));
  }
}


void setCounter(float value)
{

  if(previous_counter != value)
  {
    previous_counter = value;
    tft.setCursor(20, 80);
    tft.setTextColor(WHITE, GREEN);
    tft.setTextSize(2);
    tft.print("       ");
    tft.setCursor(20, 80);
    tft.print("FILLED " + String(value));
  }
}



void setMrnQuantity(float value){
  if(previous_mrn != value)
  {
    previous_mrn = value;
  tft.setCursor(20, 140);
  tft.setTextColor(WHITE, MAGENTA);
  tft.setTextSize(2);
  tft.print("              ");
  tft.setCursor(20, 140);
  if(value > 0)
   tft.print("MRN QTY " + String(value));
  }
}



void setRemainingQuantity(float value){
  if(previousRQ  != value)
  {
    previousRQ  = value;
    tft.setCursor(15, 200);
    tft.setTextColor(WHITE, BLUE);
    tft.setTextSize(2);
    tft.print("       ");
    tft.print("              ");
    tft.setCursor(15, 200);
    if(value > 0)
     tft.print("BAL QTY " + String(value));
  }
}



void setAuth(String value)
{

  if(previousAuth != value)
  {
    previousAuth = value;
    if(value == "AUTHENTIC" || value == "QUOTA EXCEEDED")
      tft.fillRect(10, 250, 220, 50, RED);
     else
      tft.fillRect(10, 250, 220, 50, GREEN);
    tft.drawRect(10, 250, 220, 50, WHITE);
    tft.setCursor(30, 265);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("                 ");
    tft.setCursor(30, 265);
    tft.print(value);
  }
}
