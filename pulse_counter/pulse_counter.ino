
//NJarpa
//Pulse Counter (HIGH = Pulse)

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif


#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin


boolean buttonEnabled = true;

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Pines arduino to lcd

     //-------Pins-----//
int Relay = 35;                 //Solenoid valve open/close
int start_stop = 24;             //Start/Stop button
const int sensor_pulse = 32;    // Sensor Pulse In
int rst_cnt = 28;               // Reset counter button
//---------Storage debounce function-----//
boolean currentstart_stop = LOW;          
boolean laststart_stop =LOW;            
boolean lastsensor_pulse = LOW;
boolean currentsensor_pulse = LOW;
boolean lastrst_cnt = LOW;
boolean currentrst_cnt = LOW;
boolean RelayState = LOW;              
            
int counter = 0;   

void setup() {
  pinMode(Relay, OUTPUT);
  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    identifier = 0x9341;

  }

  tft.begin(identifier);
  tft.setRotation(2);

  tft.fillScreen(BLACK);
  tft.fillRect(10, 10, 220, 50, RED);
  tft.drawRect(10, 10, 220, 50, WHITE);
  tft.setCursor(20, 20);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("COUNTER");


  tft.fillRect(10, 150, 220, 50, BLUE);
  tft.drawRect(10, 150, 220, 50, WHITE);
  tft.setCursor(20, 160);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("PULSES");

//  lcd.begin(16, 2);
//  lcd.setCursor(5, 0);
//  lcd.print("COUNTER");
//  lcd.setCursor(0, 1);
//  lcd.print("PULSES");

}
   //----Debouncing function----//
boolean debounce(boolean last, int pin)
{
boolean current = digitalRead(pin);
if (last != current)
{
delay(5);
current = digitalRead(pin);
}
return current;
}

void loop() {  
currentstart_stop = debounce(laststart_stop, start_stop);         //Debounce for Start/Stop Button
currentsensor_pulse = debounce(lastsensor_pulse, sensor_pulse);   //Debounce for Sensor pulse Button
currentrst_cnt = debounce(lastrst_cnt, rst_cnt);                 //Debounce for reset counter Button

  //-----Start/Stop toggle function----//
  if (currentstart_stop == HIGH && laststart_stop == LOW){
    
    if (RelayState == HIGH){         //Toggle the state of the Relay
      digitalWrite(Relay, LOW);
      RelayState = LOW;
    

} 
    else{
      digitalWrite(Relay, HIGH);
      RelayState = HIGH;
    }
  }
  
  laststart_stop = currentstart_stop;
  
  if (lastsensor_pulse== LOW && currentsensor_pulse == HIGH){
      counter=counter+ 1;
  }
      lastsensor_pulse = currentsensor_pulse;


tft.setCursor(25, 210);
  tft.setTextColor(0x07E0, 0x0000);
  tft.setTextSize(4);
  tft.print(counter);  
//lcd.setCursor(7, 1);
//lcd.print(counter);

if(RelayState == LOW){ //Reset counter while sistem is not running
 if (currentrst_cnt == HIGH && lastrst_cnt == LOW){//Reset Counter
  tft.setCursor(20, 210);
  tft.print("        ");
//    lcd.setCursor(6, 1);         // Clear CNT area
//    lcd.print("          ");
    counter= 0;         
   
  }
  lastrst_cnt = currentrst_cnt; 
}
 
}// end void loop
