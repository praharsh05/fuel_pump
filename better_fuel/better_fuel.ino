#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
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
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


float previous_setpoint = -1;
float previous_counter = -1;
float previous_mrn = -1;
float previousRQ = -1;
String previousAuth = "-----";

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
unsigned long currentTime;
unsigned long time_lapse;
int lastsensor_pulse;
int currentsensor_pulse;
boolean card_present_1 = false;
int flag_count = 0;
boolean card_present_2 = false;
boolean fuel_start = false;
boolean card_pre = false;
float temp_quantity;
boolean buttonEnabled = true;
//-----Variables for debouncing-----//
boolean currentstart_stop = LOW;
boolean laststart_stop = LOW;
boolean lastrst_sp = LOW;
boolean currentrst_sp = LOW;
boolean lastrst_cnt = LOW;
boolean currentrst_cnt = LOW;
boolean lastadd_one = LOW;
boolean currentadd_one = LOW;
boolean lastadd_ten = LOW;
boolean currentadd_ten = LOW;

float remainingQty;

//-----Storage state for toggle function---//
boolean RelayState = LOW;                //storage for the current state of the Relay (off/on)

int TotalCount_1 = 0;
float TotalCount_2 = 0;
int set_point_1 = 0;
int set_point_2 = 0;
String card = "C9 BD A0 C1";
String qty;
String veh ;
float quo_int;

boolean readData = false;
void setup(void) {
  pinMode(Relay, OUTPUT);     //Pin 13
  pinMode(sensor_pulse, INPUT); //flow sensor input
  pinMode(add_one, INPUT);    //A12 as Input
  pinMode(add_ten, INPUT);    //A13 as Input
  pinMode(start_stop, INPUT);

  currentTime = millis();
  lastsensor_pulse = digitalRead(sensor_pulse);
  Serial.begin(9600);   // Initiate a serial communication
  Serial1.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9341) {
  } else if (identifier == 0x0101) {
    identifier = 0x9341;
  } else {
    //    Serial.print(F("Unknown LCD driver chip: "));
    //    Serial.println(identifier, HEX);
    identifier = 0x9341;
  }

  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  lcdInit();
  lcdsetup();
}

void loop() {
  delay(500);
  if (Serial1.available() && !readData)
  {
    String c = Serial1.readString(); //makes the string readString
    Serial.println(c);
    int index1 = c.indexOf(' ');
    if (c.substring(0, index1) == "RJ14-UD-3443")
    {
      veh = c.substring(0, index1);
      Serial.println("vehicle " + veh);
      qty = c.substring(index1);
      Serial.println("Vehicle Acc " + veh + "  " + qty);
      readData = true;
      remainingQty = qty.toFloat();
    }
  }
  currentstart_stop = debounce(laststart_stop, start_stop);    //Debounce for Start/Stop Button
  currentrst_sp = debounce(lastrst_sp, rst_sp);       //Debounce for reset set point Button
  currentrst_cnt = debounce(lastrst_cnt, rst_cnt);    //Debounce for reset counter Button
  currentadd_one = debounce(lastadd_one, add_one);    //Debounce for +1 Button
  currentadd_ten = debounce(lastadd_ten, add_ten);    //Debounce for +10 Button
  Set();
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
  card_pre = look_for_card(card_present_1, card_present_2);

  //  Realay on, off handle
  realayToggle();
  laststart_stop = currentstart_stop;
  ltr_func();
  delay(100);
}

boolean look_for_card(boolean status_1, boolean status_2) {
  return status_1 || status_2;
}
