#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// LCD KEYS
int lcd_key     = 0;
int mem_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

class pump{
  private:
    int id;
    int speed1;
    int speed2;
    int speed3;
    int pin;
    int qty;
    int speed;
    int needed_time;
    int pwm;
    void calculate_values(){
      int ratio;
      if (qty > 60){
        speed=speed1;
        ratio = 3;
      }
      else if (qty > 30){
        speed=speed2;
        ratio = 2;
      }
      else {
        speed=speed3;
        ratio = 1;
      }
      needed_time = int(float(qty) / ratio * 50);
    };
    void process(){

      int passed = 0;

      lcd.setCursor(0,1);
      lcd.print("ADD 0/");
      lcd.print(qty);
      lcd.print("ml     ");
      analogWrite(pwm, speed);
      digitalWrite(pin, HIGH);
      //dynamic delay
      while (passed != needed_time) {
        delay(100);
        passed += 1;

        lcd.setCursor(0,1);
        lcd.print("ADD ");
        int done = int(float(qty) / float(needed_time) * passed);
        lcd.print(done);
        lcd.print("/");
        lcd.print(qty);
        lcd.print("ml     ");
      }
      analogWrite(pwm, 0);
      digitalWrite(pin, LOW);
      result = 1;
    };
  public:
    int result;
    void init(int target_id){
      id = target_id;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("POMPE ");
      lcd.setCursor(6,0);
      lcd.print(id);
      lcd.setCursor(0,1);
      lcd.print("INITIALISATION  ");
      pin = EEPROM.read(id);
      pwm = EEPROM.read(id + 1);
      speed1 = EEPROM.read(id + 2);
      speed2 = EEPROM.read(id + 3);
      speed3 = EEPROM.read(id + 4);

      Serial.println(pin);
      Serial.println(pwm);
      Serial.println(speed1);
      Serial.println(speed2);
      Serial.println(speed3);
    };
    int add(int todo_qty){
      qty = todo_qty;
      calculate_values();
      process();
    };
  };

pump Pompe;
void setup(){
  lcd.begin(16, 2);
  lcd.clear();

  Serial.begin(9600);
  /*
  EEPROM.write(1,1);
  EEPROM.write(2,2);
  EEPROM.write(3,230);
  EEPROM.write(4,180);
  EEPROM.write(5,130);*/
}
void loop(){
Pompe.init(1);
Pompe.add(2);

delay(2000);
}
