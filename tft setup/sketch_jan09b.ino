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
    identifier = 0x9341;

  }

  tft.begin(identifier);
  tft.setRotation(2);

  tft.fillScreen(BLACK);

  tft.fillRect(10, 10, 220, 50, RED);
  tft.drawRect(10, 10, 220, 50, WHITE);
  tft.setCursor(15, 20);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("SET VALUE");


  tft.fillRect(10, 70, 220, 50, GREEN);
  tft.drawRect(10, 70, 220, 50, WHITE);
  tft.setCursor(20, 80);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("COUNTER");


  tft.fillRect(10, 130, 220, 50, MAGENTA);
  tft.drawRect(10, 130, 220, 50, WHITE);
  tft.setCursor(20, 140);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("QUOTA");

  tft.fillRect(10, 190, 220, 50, BLUE);
  tft.drawRect(10, 190, 220, 50, WHITE);
  tft.setCursor(15, 200);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("REM QUOTA");

  tft.fillRect(10, 250, 220, 50, RED);
  tft.drawRect(10, 250, 220, 50, WHITE);
  tft.setCursor(30, 265);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("RJ14AA1000");
}
void loop() {}
