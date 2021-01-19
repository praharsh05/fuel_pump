#include <EEPROM.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
//#include <TouchScreen.h>
#include <SPI.h>
#include <MFRC522.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define SS_PIN 53
#define RST_PIN 49

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
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


//-------Pins-----//
int Relay = 35;                 //Solenoid valve open/close
int start_stop = 24;             //Start/Stop button
int rst_sp = 26;                 // Reset Set Point Button
int rst_cnt = 28;               // Reset counter button
int unit = 30;                  // Change Unit Button
const int sensor_pulse = 21;    // Sensor Pulse In
//----Analog as Input-----//
int add_one = A12;               // +1 Button
int add_ten = A13;               // +10 Button
int add_cien = A14;              // +100 Button
int add_mil = A15;               // +1000 Buton


unsigned long currentTime;
unsigned long time_lapse;
int lastsensor_pulse;
int currentsensor_pulse;

boolean card_present_1 = false;
int flag_count = 0;
boolean card_present_2 = false;
boolean fuel_start = false;

//-----Variables for debouncing-----//
boolean currentstart_stop = LOW;
boolean laststart_stop = LOW;

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
float cal_1 = 250;                      //Calibrate ml x pulse (cal_1 = 1000/400)  // Default 2.5
int cal_2 = 85;                        //Calibrate pulses x liters   // Default 400
//-----------------------------------------------------//

float counter_1 = 0.0;
int counter_2 = 0;
int TotalCount_1 = 0;
int TotalCount_2 = 0;
int set_point_1 = 0;
int set_point_2 = 0;
String quo;
int rem_quo;
String veh_no;
String qty;
String veh;
String mrn;
int quo_int;

String card = "C9 BD A0 C1";
int addr_veh = 0;
int addr_qty = 500;
int addr_mrn = 1000;

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
  pinMode(sensor_pulse, INPUT); //flow sensor input
  pinMode(add_one, INPUT);    //A12 as Input
  pinMode(add_ten, INPUT);    //A13 as Input
  pinMode(add_cien, INPUT);   //A14 as Input
  pinMode(add_mil, INPUT);    //A15 as Input
  pinMode(start_stop, INPUT);

  currentTime = millis();
  lastsensor_pulse = digitalRead(sensor_pulse);

  tft.fillRect(10, 10, 220, 50, RED);
  tft.drawRect(10, 10, 220, 50, WHITE);
  tft.setCursor(15, 20);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("       ");

  tft.fillRect(10, 70, 220, 50, GREEN);
  tft.drawRect(10, 70, 220, 50, WHITE);
  tft.setCursor(20, 80);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("       ");

  tft.fillRect(10, 130, 220, 50, MAGENTA);
  tft.drawRect(10, 130, 220, 50, WHITE);
  tft.setCursor(20, 140);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("       ");

  tft.fillRect(10, 190, 220, 50, BLUE);
  tft.drawRect(10, 190, 220, 50, WHITE);
  tft.setCursor(15, 200);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("       ");

  tft.fillRect(10, 250, 220, 50, RED);
  tft.drawRect(10, 250, 220, 50, WHITE);
  tft.setCursor(30, 265);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("AUTHENTIC");

  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  //  while (Serial.available()) // this will be skipped if no data present, leading to
  //    // the code sitting in the delay function below
  //  {

}



void loop() {

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


    tft.setCursor(15, 20);
    tft.setTextColor(0xFFFF, 0xF800);
    tft.setTextSize(4);
    tft.println(set_point_1);//Show set point
  }
  //    Serial.println("new loop\n");
  while (flag_count < 2) {
    if (flag_count == 0) {
      card_present_1 = check_card(card);

    }
    else if (flag_count == 1) {
      card_present_2 = check_card(card);
    }
    flag_count++;
  }
  flag_count = 0;

  boolean card_pre = look_for_card(card_present_1, card_present_2);
  //    Serial.println(card_pre);
  //return;

  //  if (Serial.available()) // this will be skipped if no data present, leading to
  //    // the code sitting in the delay function below
  //  {
  //    delay(30);  //delay to allow buffer to fill
  //
  //
  //    //      String c = Serial.read();  //gets one byte from serial buffer
  //    String c = Serial.readString(); //makes the string readString
  //    Serial.println("data: " + c);
  //    //    int ind1 = Serial.readString().indexOf(' ');  //finds location of first ,
  //    //    veh = Serial.readString().substring(0, ind1);   //captures first data String
  //    //    int ind2 = Serial.readString().indexOf(' ', ind1 + 1 );
  //    //    qty = Serial.readString().substring(' ', ind1 + 1); //captures second data String
  //    //    int ind3 = Serial.readString().indexOf('\n', ind2 + 1 );
  //    //    mrn = Serial.readString().substring(ind2 + 1, ind3); //captures third data String
  //
  //
  //    EEPROM.write(addr_veh, veh.toInt());
  //    EEPROM.write(addr_qty, qty.toInt());
  //
  //  }
  delay(30);  //delay to allow buffer to fill
  if (Serial.available() > 0)
  {
    String c = Serial.readString(); //makes the string readString
    Serial.println("data: " + c);
    writeString(addr_veh, c);
    qty = Serial.readString();
    Serial.println("qty: " + qty);
    writeString(addr_qty, qty);

  }

  //  }
  //-----Debounce Buttons-----//
  currentstart_stop = debounce(laststart_stop, start_stop);    //Debounce for Start/Stop Button
  //  currentsensor_pulse = debounce(lastsensor_pulse, sensor_pulse); //Debounce for Sensor
  currentunit = debounce(lastunit, unit);             //Debounce for unit Button
  currentrst_sp = debounce(lastrst_sp, rst_sp);       //Debounce for reset set point Button
  currentrst_cnt = debounce(lastrst_cnt, rst_cnt);    //Debounce for reset counter Button
  currentadd_one = debounce(lastadd_one, add_one);    //Debounce for +1 Button
  currentadd_ten = debounce(lastadd_ten, add_ten);    //Debounce for +10 Button
  currentadd_cien = debounce(lastadd_cien, add_cien); //Debounce for +100 Button
  currentadd_mil = debounce(lastadd_mil, add_mil);    //Debounce for +1000 Button

  //digitalWrite(Relay,LOW);
  //  -----Start/Stop toggle function----//
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
      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x07E0);//change to white
      tft.setTextSize(4);
      tft.println("       ");//Clear tft(CNT area) between unit change,keeping last count
      tft.fillRect(10, 250, 220, 50, RED);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("AUTHENTIC");

      tft.setCursor(15, 20);
      tft.setTextColor(0xFFFF, 0xF800);
      tft.setTextSize(4);
      tft.println("       ");//Clear lcd (SP area) between unit change, keeping last SP

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


    tft.setCursor(190, 20);
    tft.setTextColor(0xFFFF, 0xF800);
    tft.setTextSize(3);
    tft.println("Lt");

    tft.setCursor(190, 80);
    tft.setTextColor(0xFFFF, 0x07E0);
    tft.setTextSize(3);
    tft.println("Lt");

    tft.setCursor(190, 140);
    tft.setTextColor(0xFFFF, 0xF81F);
    tft.setTextSize(3);
    tft.println("Lt");

    tft.setCursor(190, 200);
    tft.setTextColor(0xFFFF, 0x001F);
    tft.setTextSize(3);
    tft.println("Lt");

  }
  else {                      //Unit state LOW = ml

    tft.setCursor(190, 20);
    tft.setTextColor(0xFFFF, 0xF800);
    tft.setTextSize(3);
    tft.println("Ml");

    tft.setCursor(190, 80);
    tft.setTextColor(0xFFFF, 0x07E0);
    tft.setTextSize(3);
    tft.println("Ml");

    tft.setCursor(190, 140);
    tft.setTextColor(0xFFFF, 0xF81F);
    tft.setTextSize(3);
    tft.println("Ml");

    tft.setCursor(190, 200);
    tft.setTextColor(0xFFFF, 0x001F);
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

      //      //-----Adders Buttons (set point_1)---//
      //
      //      if (currentadd_ten == HIGH && lastadd_ten == LOW) { // Add +10
      //        set_point_1 = set_point_1 + 10;
      //      }
      //      lastadd_ten = currentadd_ten;
      //
      //      if (currentadd_cien == HIGH && lastadd_cien == LOW) { // Add +100
      //        set_point_1 = set_point_1 + 100;
      //      }
      //      lastadd_cien = currentadd_cien;
      //
      //      if (currentadd_mil == HIGH && lastadd_mil == LOW) { // Add +1000
      //        set_point_1 = set_point_1 + 1000;
      //      }
      //      lastadd_mil = currentadd_mil;

      //-------Reset Buttons----//
      if (currentrst_sp == HIGH && lastrst_sp == LOW) { //Reset Set Point
        tft.setCursor(15, 20);
        tft.setTextColor(0xFFFF, 0xF800);
        tft.setTextSize(4);
        tft.println("       ");// Clear SP area
        set_point_1 = 0;
      }
      lastrst_sp = currentrst_sp;
      if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
        tft.setCursor(20, 80);
        tft.setTextColor(0xFFFF, 0x07E0);
        tft.setTextSize(4);
        tft.println("       ");// Clear CNT area
        tft.fillRect(10, 250, 220, 50, RED);
        tft.drawRect(10, 250, 220, 50, WHITE);
        tft.setCursor(30, 265);
        tft.setTextColor(WHITE);
        tft.setTextSize(3);
        tft.print("AUTHENTIC");
        counter_1 = 0;
        TotalCount_1 = 0;
      }
      lastrst_cnt = currentrst_cnt;
    }//-----End Settings-----//


    //    tft.setCursor(15, 20);
    //    tft.setTextColor(0xFFFF, 0xF800);
    //    tft.setTextSize(4);
    //    tft.println(set_point_1);//Show set point
    //    Serial.print(String(set_point_1) + " " );
    //RelayState=LOW;
    // Look for new cards

    if (card_pre) {
      tft.setCursor(30, 265);
      tft.setTextColor(RED);
      tft.setTextSize(3);
      tft.println("          ");// Clear Authenticate Area
      tft.fillRect(10, 250, 220, 50, GREEN);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      veh_no =  read_String(addr_veh);
      tft.print(veh_no);
      //        Serial.print(String(veh_no) + " ");
      tft.fillRect(10, 130, 220, 50, MAGENTA);
      tft.drawRect(10, 130, 220, 50, WHITE);
      tft.setCursor(20, 140);
      tft.setTextColor(WHITE);
      tft.setTextSize(4);
      quo = read_String(addr_qty);
      tft.print(quo);
      quo_int = quo.toInt();
      //----Start Counter------//
      if (RelayState == HIGH) {  // Only counts while relay is HIGH
        //          Serial.println("relay on");
        if (!fuel_start) {
          currentTime = millis();
          fuel_start = true;
        }

        time_lapse = millis() - currentTime;

        //          Serial.println(time_lapse);
        //          Serial.println(int(time_lapse / 250));

        TotalCount_1 = int(time_lapse / 250) * 10;
        //          Serial.println(TotalCount_1);
        tft.setCursor(20, 80);
        tft.setTextColor(0xFFFF, 0x07E0);
        tft.setTextSize(4);
        tft.println(TotalCount_1);// Show counter



        //--Stop Counter.You can't start if set point is lower or equal to counter--//
        if (RelayState == HIGH) {
          //            Serial.println("total " + String(TotalCount_1) + " set point " + String(set_point_1));
          if (quo_int < set_point_1) {
            RelayState = LOW;
            digitalWrite(Relay, LOW);
            tft.setCursor(15, 20);
            tft.setTextColor(0xFFFF, 0xF800);
            tft.setTextSize(4);
            tft.println("       ");// Clear SP area
            tft.setCursor(20, 80);
            tft.setTextColor(0xFFFF, 0x07E0);
            tft.setTextSize(4);
            tft.println("       ");// Clear CNT area
            tft.fillRect(10, 250, 220, 50, RED);
            tft.drawRect(10, 250, 220, 50, WHITE);
            tft.setCursor(30, 265);
            tft.setTextColor(WHITE);
            tft.setTextSize(3);
            tft.print("AUTHENTIC");

            TotalCount_1 = 0;
            set_point_1 = 0;
            fuel_start = false;
          }
          if (set_point_1 <= TotalCount_1) {
            RelayState = LOW;
            digitalWrite(Relay, LOW);
            tft.fillRect(10, 190, 220, 50, BLUE);
            tft.drawRect(10, 190, 220, 50, WHITE);
            tft.setCursor(15, 200);
            tft.setTextColor(WHITE);
            tft.setTextSize(4);
            tft.print(quo_int - TotalCount_1);
            //***********************************************Autoreset
            tft.setCursor(20, 80);
            tft.setTextColor(0xFFFF, 0x07E0);
            tft.setTextSize(4);
            tft.println("       ");// Clear CNT area
            tft.fillRect(10, 250, 220, 50, RED);
            tft.drawRect(10, 250, 220, 50, WHITE);
            tft.setCursor(30, 265);
            tft.setTextColor(WHITE);
            tft.setTextSize(3);
            tft.print("AUTHENTIC");

            counter_1 = 0;
            TotalCount_1 = 0;
            fuel_start = false;
          }

        }
      }
    }
    else {
      RelayState = LOW;
      digitalWrite(Relay, LOW);
      //***********************************************Autoreset
      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x07E0);
      tft.setTextSize(4);
      tft.println("       ");// Clear CNT area
      tft.fillRect(10, 250, 220, 50, RED);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("AUTHENTIC");

      counter_1 = 0;
      TotalCount_1 = 0;
      fuel_start = false;
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
        tft.setCursor(15, 20);
        tft.setTextColor(0xFFFF, 0xF800);
        tft.setTextSize(4);
        tft.println("       ");// Clear SP area
        set_point_2 = 0;
      }
      lastrst_sp = currentrst_sp;
      if (currentrst_cnt == HIGH && lastrst_cnt == LOW) { //Reset Counter
        tft.setCursor(20, 80);
        tft.setTextColor(0xFFFF, 0x07E0);
        tft.setTextSize(4);
        tft.println("       ");// Clear CNT area
        counter_2 = 0;
        TotalCount_2 = 0;
      }
      lastrst_cnt = currentrst_cnt;
    }//-----End Settings-----//


    tft.setCursor(15, 20);
    tft.setTextColor(0xFFFF, 0xF800);
    tft.setTextSize(4);
    tft.println(set_point_1);//Show set point


    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Show UID on serial monitor
    //    Serial.print("UID tag :");
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1) == "C9 BD A0 C1") //change here the UID of the card/cards that you want to give access
    {
      tft.setCursor(30, 265);
      tft.setTextColor(RED);
      tft.setTextSize(3);
      tft.println("          ");// Clear Authenticate Area
      tft.fillRect(10, 250, 220, 50, GREEN);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("RJ14AA0000");
      //      Serial.println("Authorized access");
      //      Serial.println();
      delay(3000);
      //----Start Counter------//
      if (RelayState == HIGH) {  // Only counts while relay is HIGH
        currentTime = millis();
        if (lastsensor_pulse == 0 && currentsensor_pulse == 1) {
          counter_2 = counter_2 + 1;
          if ( ! mfrc522.PICC_ReadCardSerial())
          {
            return;
          }
          String content = "";
          byte letter;
          for (byte i = 0; i < mfrc522.uid.size; i++)
          {
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
          }
          content.toUpperCase();
          if (content.substring(1) != "C9 BD A0 C1")
            digitalWrite(Relay, LOW);

        }
      }
      lastsensor_pulse = currentsensor_pulse;
      time_lapse = millis() - currentTime;

      //-------Counter function-----//
      if (time_lapse >= 25000) {
        TotalCount_2 = TotalCount_2 + 1;
        time_lapse = 0;                     //Counter  reset
      }

      tft.setCursor(20, 80);
      tft.setTextColor(0xFFFF, 0x07E0);
      tft.setTextSize(4);
      tft.println(TotalCount_2);// Show counter


      //--Stop Counter.You can¥t start if set point is lower or equal to counter--//
      if (RelayState == HIGH) {
        if (set_point_2 <= TotalCount_2) {
          RelayState = LOW;
          digitalWrite(Relay, LOW);
          //*****************************Autoreset

          tft.setCursor(20, 80);
          tft.setTextColor(0xFFFF, 0x07E0);
          tft.setTextSize(4);
          tft.println("       ");// Clear CNT area
          tft.fillRect(10, 250, 220, 50, RED);
          tft.drawRect(10, 250, 220, 50, WHITE);
          tft.setCursor(30, 265);
          tft.setTextColor(WHITE);
          tft.setTextSize(3);
          tft.print("AUTHENTIC");

          counter_2 = 0;
          TotalCount_2 = 0;
        }
      }
    }
    else {
      tft.fillRect(10, 250, 220, 50, RED);
      tft.drawRect(10, 250, 220, 50, WHITE);
      tft.setCursor(30, 265);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("RJ14AB0001");
      delay(3000);
      tft.setCursor(30, 265);
      tft.setTextColor(RED);
      tft.setTextSize(4);
      tft.println("          ");
      tft.setCursor(30, 265);
      tft.setTextColor(BLACK);
      tft.setTextSize(3);
      tft.print("DENIED");
      //      Serial.println(" Access denied");
      delay(3000);
    }
  }//End unit state HIGH (L)
  delay(100);
  Serial.println(digitalRead(Relay));
}//End Void Loop

void writeString(int add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  return ;
}


String read_String(int add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
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
void stop_relay() {
  Serial.println("no card");
  tft.fillRect(10, 250, 220, 50, RED);
  tft.drawRect(10, 250, 220, 50, WHITE);
  tft.setCursor(30, 265);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("RJ14AB0001");
  //        delay(3000);
  tft.setCursor(30, 265);
  tft.setTextColor(RED);
  tft.setTextSize(4);
  tft.println("          ");
  tft.setCursor(30, 265);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("DENIED");
  digitalWrite(Relay, LOW);
  RelayState = LOW;
}

boolean look_for_card(boolean status_1, boolean status_2) {
  return status_1 || status_2;
}

boolean check_card(String card) {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == card) {
    return true;
  }
  else {
    return false;
  }
}
