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

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);



//-------Pins-----//
int Relay = 22;                 //Solenoid valve open/close
int start_stop = 24;             //Start/Stop button
int rst_sp = 26;                 // Reset Set Point Button
int rst_cnt = 28;               // Reset counter button
int unit = 30;                  // Change Unit Button
const int sensor_pulse = 32;    // Sensor Pulse In
//----Analog as Input-----//
int add_one = 66;               // +1 Button
int add_ten = 67;               // +10 Button
int add_cien = 68;              // +100 Button
int add_mil = 69;               // +1000 Buton

//-----Variables for debouncing-----//
boolean currentstart_stop = LOW;
boolean laststart_stop = LOW;
boolean lastsensor_pulse = LOW;
boolean currentsensor_pulse = LOW;
boolean lastunit = LOW;
boolean currentunit = LOW;
boolean lastrst_sp = LOW;
boolean currentrst_sp = LOW;
boolean lastrst_cnt = LOW;
boolean currentrst_cnt = LOW;
boolean lastadd_one = LOW;
boolean currentadd_one = LOW;
boolean lastadd_ten = LOW;
boolean currentadd_ten = LOW;
boolean lastadd_cien = LOW;
boolean currentadd_cien = LOW;
boolean lastadd_mil = LOW;
boolean currentadd_mil = LOW;

//-----Storage state for toggle function---//
boolean unitState = LOW;                  //storage for the current state of the unit
boolean RelayState = LOW;                //storage for the current state of the Relay (off/on)

//-------You have to put your pulses x liters here-----//
float cal_1 = 2.5;                      //Calibrate ml x pulse (cal_1 = 1000/400)  // Default 2.5
int cal_2 = 400;                        //Calibrate pulses x liters   // Default 400
//-----------------------------------------------------//

float counter_1 = 0.0;
int counter_2 = 0;
int TotalCount_1 = 0;
int TotalCount_2 = 0;
int set_point_1 = 0;
int set_point_2 = 0;


void setup(void) { //body

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
  pinMode(Relay, OUTPUT);     //Pin 13
  pinMode(add_one, INPUT);    //A12 as Input
  pinMode(add_ten, INPUT);    //A13 as Input
  pinMode(add_cien, INPUT);   //A14 as Input
  pinMode(add_mil, INPUT);    //A15 as Input


  tft.fillRect(10, 10, 220, 50, RED);
  tft.drawRect(10, 10, 220, 50, WHITE);
  tft.setCursor(20, 20);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("SetValue");


  tft.fillRect(10, 150, 220, 50, BLUE);
  tft.drawRect(10, 150, 220, 50, WHITE);
  tft.setCursor(20, 160);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("COUNTER");

}
//----Debouncing function for all buttons----//
boolean debounce(boolean last, int pin)
{
  boolean current = digitalRead(pin);
  if (last != current)
  {
    delay(10);
    current = digitalRead(pin);
  }
  return current;
}

void loop() {
  //-----Debounce Buttons-----//
  currentstart_stop = debounce(laststart_stop, start_stop);    //Debounce for Start/Stop Button
  currentsensor_pulse = debounce(lastsensor_pulse, sensor_pulse); //Debounce for Sensor
  currentunit = debounce(lastunit, unit);             //Debounce for unit Button
  currentrst_sp = debounce(lastrst_sp, rst_sp);       //Debounce for reset set point Button
  currentrst_cnt = debounce(lastrst_cnt, rst_cnt);    //Debounce for reset counter Button
  currentadd_one = debounce(lastadd_one, add_one);    //Debounce for +1 Button
  currentadd_ten = debounce(lastadd_ten, add_ten);    //Debounce for +10 Button
  currentadd_cien = debounce(lastadd_cien, add_cien); //Debounce for +100 Button
  currentadd_mil = debounce(lastadd_mil, add_mil);    //Debounce for +1000 Button


  //-----Start/Stop toggle function----//
  if (currentstart_stop == HIGH && laststart_stop == LOW) {

    if (RelayState == HIGH) {        //Toggle the state of the Relay
      digitalWrite(Relay, LOW);
      RelayState = LOW;
    }
    else {
      digitalWrite(Relay, HIGH);
      RelayState = HIGH;
    }
  }

  laststart_stop = currentstart_stop;

  //-------Unit toggle function----//
  if (RelayState == LOW) {          //You only can change unit while system is not running!

    //------ Lt/ml unit toggle function----//
    if (currentunit == HIGH && lastunit == LOW) {
      // lcd.setCursor(4, 1);        //Clear lcd(CNT area) between unit change,keeping last count
      // lcd.print("          ");

      tft.setCursor(10, 220);
      tft.setTextColor(0xFFFF, 0x0000);
      tft.setTextSize(5);
      tft.println("       ");



      // lcd.setCursor(3,0);        //Clear lcd (SP area) between unit change, keeping last SP
      // lcd.print("           ");

      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x0000);
      tft.setTextSize(5);
      tft.println("       ");





      if (unitState == HIGH) {        //Toggle the state of the unit (L/ml)
        digitalWrite(unit, LOW);
        unitState = LOW;
      }
      else {
        digitalWrite(unit, HIGH);
        unitState = HIGH;
      }
    }
    lastunit = currentunit;
  }
  //------Print unit state-----//
  if (unitState == HIGH) {    //Unit state HIGH = L
    //lcd.setCursor(14,0);
    //lcd.print("Lt");


    tft.setCursor(200, 90);
    tft.setTextColor(0xF81F, 0x0000);
    tft.setTextSize(3);
    tft.println("Lt");


    //lcd.setCursor(14, 1);
    //lcd.print("Lt");


    tft.setCursor(200, 230);
    tft.setTextColor(0xF81F, 0x0000);
    tft.setTextSize(3);
    tft.println("Lt");

  }
  else {                      //Unit state LOW = ml
    // lcd.setCursor(14,0);
    //  lcd.print("Ml");


    tft.setCursor(200, 90);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(3);
    tft.println("Ml");


    // lcd.setCursor(14,1);
    // lcd.print("Ml");


    tft.setCursor(200, 230);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(3);
    tft.println("Ml");



  }//End Print unit state

  //--------------------------//
  //------------Ml Counter-----//
  //---------------------------//
  if (unitState == LOW) {  // LOW= Ml state

    //-----------------------//
    //-----Settings----------//
    //----------------------//

    if (RelayState == LOW) {   // You only can change settings while system is not running!

      //-----Adders Buttons (set point_1)---//

      if (currentadd_ten == HIGH && lastadd_ten == LOW) { // Add +10
        set_point_1 = set_point_1 + 10;
      }
      lastadd_ten = currentadd_ten;

      if (currentadd_cien == HIGH && lastadd_cien == LOW) { // Add +100
        set_point_1 = set_point_1 + 100;
      }
      lastadd_cien = currentadd_cien;

      if (currentadd_mil == HIGH && lastadd_mil == LOW) { // Add +1000
        set_point_1 = set_point_1 + 1000;
      }
      lastadd_mil = currentadd_mil;

      //-------Reset Buttons----//
      if (currentrst_sp == HIGH && lastrst_sp == LOW) { //Reset Set Point
        // lcd.setCursor(3, 0);         // Clear SP area
        //lcd.print("          ");

        tft.setCursor(10, 80);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.setTextSize(5);
        tft.println("       ");


        set_point_1 = 0;
      }
      lastrst_sp = currentrst_sp;
      if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
        // lcd.setCursor(4, 1);         // Clear CNT area
        // lcd.print("         ");

        tft.setCursor(10, 220);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.setTextSize(5);
        tft.println("       ");



        counter_1 = 0;
        TotalCount_1 = 0;
      }
      lastrst_cnt = currentrst_cnt;
    }//-----End Settings-----//

    //----Start Counter------//
    if (RelayState == HIGH) {  // Only counts while relay is HIGH
      if (lastsensor_pulse == LOW && currentsensor_pulse == HIGH) {
        counter_1 = counter_1 + cal_1;
      }
    }
    lastsensor_pulse = currentsensor_pulse;

    //-------Counter function-----//
    if (counter_1 >= 10) {
      TotalCount_1 = TotalCount_1 + 10;
      counter_1 = 0;                   //Counter  reset
    }

    // lcd.setCursor(3, 0);           //Show set point
    //lcd.print(set_point_1);

    tft.setCursor(10, 80);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(5);
    tft.println(set_point_1);


    // lcd.setCursor(4, 1);           // Show counter
    //lcd.print(TotalCount_1);

    tft.setCursor(10, 220);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(5);
    tft.println(TotalCount_1);




    //--Stop Counter.You can¥t start if set point is lower or equal to counter--//
    if (RelayState == HIGH) {
      if (set_point_1 <= TotalCount_1) {
        RelayState = LOW;
        digitalWrite(Relay, LOW);
        //***********************************************Autoreset
        // lcd.setCursor(4, 1);         // Clear CNT area
        // lcd.print(set_point_1);

        tft.setCursor(10, 220);
        tft.setTextColor(0x07E0, 0x0000);
        tft.setTextSize(5);
        tft.println(set_point_1);

        // counter_1= 0;
        // TotalCount_1= 0;






      }
    }
  }//End unit state LOW (ml)

  //--------------------------//
  //------------Lt Counter-----//
  //---------------------------//

  if (unitState == HIGH) {   //HIGH = Lt state

    //-----------------------//
    //-----Settings----------//
    //----------------------//

    if (RelayState == LOW) {   // You only can change settings while system is not running!

      //-----Adders Buttons (set point_2)---//
      if (currentadd_one == HIGH && lastadd_one == LOW) { // Add +1
        set_point_2 = set_point_2 + 1;
      }
      lastadd_one = currentadd_one;

      if (currentadd_ten == HIGH && lastadd_ten == LOW) { // Add +10
        set_point_2 = set_point_2 + 10;
      }
      lastadd_ten = currentadd_ten;

      if (currentadd_cien == HIGH && lastadd_cien == LOW) { // Add +100
        set_point_2 = set_point_2 + 100;
      }
      lastadd_cien = currentadd_cien;

      if (currentadd_mil == HIGH && lastadd_mil == LOW) { // Add +1000
        set_point_2 = set_point_2 + 1000;
      }
      lastadd_mil = currentadd_mil;

      //-------Reset Buttons----//
      if (currentrst_sp == HIGH && lastrst_sp == LOW) { //Reset Set Point
        // lcd.setCursor(3, 0);         // Clear SP area
        // lcd.print("          ");

        tft.setCursor(10, 80);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.setTextSize(5);/////////////
        tft.println("       ");





        set_point_2 = 0;
      }
      lastrst_sp = currentrst_sp;
      if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
        // lcd.setCursor(4, 1);         // Clear CNT area
        // lcd.print("         ");


        tft.setCursor(10, 220);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.setTextSize(5);
        tft.println("       ");



        counter_2 = 0;
        TotalCount_2 = 0;
      }
      lastrst_cnt = currentrst_cnt;
    }//-----End Settings-----//

    //----Start Counter------//
    if (RelayState == HIGH) {  // Only counts while relay is HIGH
      if (lastsensor_pulse == LOW && currentsensor_pulse == HIGH) {
        counter_2 = counter_2 + 1;
      }
    }
    lastsensor_pulse = currentsensor_pulse;

    //-------Counter function-----//
    if (counter_2 == cal_2) {
      TotalCount_2 = TotalCount_2 + 1;
      counter_2 = 0;                     //Counter  reset
    }

    // lcd.setCursor(3, 0);           //Show set point
    // lcd.print(set_point_2);

    tft.setCursor(10, 80);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(5);
    tft.println(set_point_2);


    // lcd.setCursor(4, 1);           // Show counter
    // lcd.print(TotalCount_2);

    tft.setCursor(10, 220);
    tft.setTextColor(0x07E0, 0x0000);
    tft.setTextSize(5);
    tft.println(TotalCount_2);


    //--Stop Counter.You can¥t start if set point is lower or equal to counter--//
    if (RelayState == HIGH) {
      if (set_point_2 <= TotalCount_2) {
        RelayState = LOW;
        digitalWrite(Relay, LOW);
        //*****************************Autoreset
        // lcd.setCursor(4, 1);         // Clear CNT area
        // lcd.print("         ");


        tft.setCursor(10, 220);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.setTextSize(5);
        tft.println("       ");





        counter_2 = 0;
        TotalCount_2 = 0;
      }
    }


  }//End unit state HIGH (L)



}//End Void Loop
