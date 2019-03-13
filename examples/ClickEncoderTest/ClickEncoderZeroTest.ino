#define WITH_LCD 1

#include <ClickEncoderZero.h>
#include <ZeroTimer.h>

#ifdef WITH_LCD
#include <LiquidCrystal.h>

#define LCD_RS       8
#define LCD_RW       9
#define LCD_EN      10
#define LCD_D4       4
#define LCD_D5       5
#define LCD_D6       6
#define LCD_D7       7

#define LCD_CHARS   20
#define LCD_LINES    4

LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif

ClickEncoderZero *encoder;
int16_t last, value;

void timerIsr() {
  encoder->service();
}

#ifdef WITH_LCD
void displayAccelerationStatus() {
  lcd.setCursor(0, 1);  
  lcd.print("Acceleration ");
  lcd.print(encoder->getAccelerationEnabled() ? "on " : "off");
}
#endif

void setup() {
  Serial.begin(9600);
  encoder = new ClickEncoderZero(A1, A0, A2);

#ifdef WITH_LCD
  lcd.begin(LCD_CHARS, LCD_LINES);
  lcd.clear();
  displayAccelerationStatus();
#endif

  TCC.startTimer(1000, timerIsr);
  
  last = -1;
}

void loop() {  
  value += encoder->getValue();
  
  if (value != last) {
    last = value;
    Serial.print("Encoder Value: ");
    Serial.println(value);
#ifdef WITH_LCD
    lcd.setCursor(0, 0);
    lcd.print("         ");
    lcd.setCursor(0, 0);
    lcd.print(value);
#endif
  }
  
  ClickEncoderZero::Button b = encoder->getButton();
  if (b != ClickEncoderZero::Open) {
    Serial.print("Button: ");
    #define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b) {
      VERBOSECASE(ClickEncoderZero::Pressed);
      VERBOSECASE(ClickEncoderZero::Held)
      VERBOSECASE(ClickEncoderZero::Released)
      VERBOSECASE(ClickEncoderZero::Clicked)
      case ClickEncoderZero::DoubleClicked:
          Serial.println("ClickEncoderZero::DoubleClicked");
          encoder->setAccelerationEnabled(!encoder->getAccelerationEnabled());
          Serial.print("  Acceleration is ");
          Serial.println((encoder->getAccelerationEnabled()) ? "enabled" : "disabled");
#ifdef WITH_LCD
          displayAccelerationStatus();
#endif
        break;
    }
  }    
}

