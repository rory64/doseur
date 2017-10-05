#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;

bool arrosage;
bool in_menu;

// EEPROM ADDRESSES
// Application data
#define WATER_PUMP_PIN            1
#define WATER_QTY_PIN             2
#define SENSOR_DELAY_PIN          3
// Definition and calibration of pumps
#define PUMP_START_PIN            10
// Culture phases
#define PHASE_START_PIN           60
#define PHASE_LINE_START_PIN      90
// Components
#define COMPONENT_START_PIN       270 // 20 addresses by component

//DEFINE WORDS SIZE
int PUMP_INVENTORY[4] = {10,} ;           
// Default values
#define DEFAULT_WATER_PUMP    1
#define DEFAULT_WATER_QTY     2
#define DEFAULT_SENSOR_DELAY  180
#define DEFAULT_SPEED1        250
#define DEFAULT_SPEED2        190
#define DEFAULT_SPEED3        130

#define UP_KEY                1
#define RIGHT_KEY             2
#define DOWN_KEY              3
#define LEFT_KEY              4
#define SELECT_KEY            5


void usine(){
  EEPROM.write(WATER_PUMP_PIN, DEFAULT_WATER_PUMP);
  EEPROM.write(WATER_QTY_PIN, DEFAULT_WATER_QTY);
  EEPROM.write(SENSOR_DELAY_PIN, DEFAULT_SENSOR_DELAY);
};

class phase{};

class phase_line{};

class component{
  private:
    int id;
    char name;
    int pump_id;
    int default_qty;

    char get_component_name(){
      return "TEST DE NAME ---";
    };
  public:
    init(int component_id){    
      id = component_id;  
      name = get_component_name();
      pump_id = EEPROM.read(id + 17);
      default_qty = EEPROM.read(id + 18);
    }
  };

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
    pump(int target_id){
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

class preparation{};



const char* messageFactory[20] = {
  /* 0   */ "HOME",
  /* 1   */ "COMPOSANTS",
  /* 2   */ "REGLAGES",
  /* 3   */ "SETTINGS",
  /* 4   */ "NAME",
  /* 5   */ "PUMP",
  /* 6   */ "QTY",
  /* 7   */ "WATER PUMP ID",
  /* 8   */ "WATER QTY",
  /* 9   */ "SENSOR DELAY",
  /* 10   */ "Select > edit",
  /* 11   */ "Select_confirm",
  /* 12   */ "CALIBRATION",
  /* 13   */ "FACTORY SETTINGS",
  /* 14   */ "DELAY",
  /* 15   */ "CULTURE",
  /* 16   */ ""
};

typedef struct menuItem{
  public:
    // Values
    int title;
    int subtitle;
    int parent_id;
    int up_id;
    int down_id;
    int select_id;
    int widget_id;
    int address;
    int max_val;
    bool nullable;
    // Program
} menuItem;


const menuItem structure[14] =  {
  // id  description             title, subtitle,parent_id, up_id, down_id, select_id, widget,            address, max_val, nullable
  /* 0   HOME                */{     0,       14,       -1,    -1,      -1,         1,      0,                  0,       0,        0  },
  /* 1   REGLAGES CULTURE    */{     2,       15,        0,     1,       2,         8,      0,                  0,       0,        0  },
  /* 2   REGLAGES COMPOSANTS */{     2,        1,        0,     1,       3,         4,      0,                  0,       0,        0  },
  /* 3   REGLAGES SETTINGS   */{     2,        3,        0,     2,       3,        11,      0,                  0,       0,        0  },
  /* 4   COMPOSANTS          */{     1,       16,        2,    -1,      -1,         5,      0,                  0,       0,        0  }, 
  /* 5     NAME              */{     4,       16,        4,    -1,       6,        -1,      0,                  0,      10,        0  },
  /* 6     PUMP              */{     5,       16,        4,     5,       7,        -1,      0,                  0,       4,        1  },
  /* 7     QTY               */{     6,       16,        4,     6,      -1,        -1,      0,                  0,     255,        0  },
  //WATER SETTINGS
  /* 8   WATER PUMP ID       */{     7,       10,        1,    -1,       9,        -1,      1,WATER_PUMP_PIN     ,       4,        0  }, 
  /* 9   WATER QTY           */{     8,       10,        1,     8,      10,        -1,      1,WATER_QTY_PIN      ,       5,        0  },
  /* 10  SENSOR DELAY        */{     9,       10,        1,     9,      -1,        -1,      1,SENSOR_DELAY_PIN   ,     255,        0  },
  /* 11  CALIBRATION         */{    12,       11,        3,    -1,       12,       -1,      1,                  2,       0,        0  },
  /* 12  FACTORY SETTINGS    */{    13,       12,        3,    11,      -1,         6,      5,                  2,       0,        0  }
};


class menuWidget{
  protected:
      int menu;
      void set_title(char text[16]){
      lcd.setCursor(0,0);
      lcd.print(fill_line(text));
    }
    void set_subtitle(char text[16]){
        lcd.setCursor(0,1);
        lcd.print(fill_line(text));
    }  
    String fill_line(char text[16]){
      int len = String(text).length();
      String message;
      while (message.length() < (16 - len) / 2){
        message.concat(" ");
      }
      message.concat(text);
      while (message.length() < 16){
        message.concat(" ");
      }  
      return message;    
    }
  private:
    
    int up_action(){
      int next_id;
      next_id = structure[menu].up_id; 
      return next_id;
    }
    int right_action(){
      return -1;
    }
    int down_action(){
      int next_id;
      next_id = structure[menu].down_id; 
      return next_id;
    }
    int left_action(){
      int next_id;
      next_id = structure[menu].parent_id; 
      return next_id;
    }
    int select_action(){
      int next_id;
      next_id = structure[menu].select_id; 
      return next_id;
    } 
    
  public:
    void set_menu(int menu_id){
      Serial.println(menu_id);
      menu = menu_id;
      set_title(messageFactory[structure[menu].title]);
      set_subtitle(messageFactory[structure[menu].subtitle]);
    }
    int key_signal(int key){
      switch(key){
        case UP_KEY:
          return up_action();
        case RIGHT_KEY:
          return right_action();
          break;
        case DOWN_KEY:
          return down_action();
          break;
        case LEFT_KEY:
          return left_action();
          break;
        case SELECT_KEY:
          return select_action();
          break;
      }
    }
};

class menuWidgetInteger: public menuWidget{
  private:
    int editing = 0; 
    int eeprom_address = 0;
    int max_val = 0;
    bool nullable = 0;
    void confirm_editing(){
      editing = 1;
      Serial.println("Start editing");
      show_integer_widget();      
    }
    void exit_editing(){
      editing = 0;      
      Serial.println("End editing");
    }
    // integer ml : update qty
    // Refresh value after update
    void update_eeprom_value(int ml=0){
      Serial.println("Update EEPROM Value");
      int val = EEPROM.read(eeprom_address);
      if (1 - nullable <= val + ml &&  val + ml <= max_val) {
        EEPROM.write(eeprom_address, val + ml); 
      }
      show_integer_widget();
    }
    void show_integer_widget(){
      Serial.println("Start show widget");
        int val = EEPROM.read(eeprom_address);
        String val_str = String(val, DEC);
        int val_len = val_str.length();
        
        String message;
        if (val > 1 - nullable){ message.concat("-");}
        else { message.concat(" ");}
        message.concat("     ");
        if (val_len < 4){ message.concat(" ");}
        if (val_len == 1){ message.concat(" ");}
        message.concat(val);
        if (val_len == 2){ message.concat(" ");}
        if (val_len == 1){ message.concat(" ");}
        message.concat("     ");
        if (max_val == val){ message.concat(" ");}
        else { message.concat("+");}

        char text[17];
        message.toCharArray(text,17);
        set_subtitle(text);
        Serial.println("Stop show widget");
    }
    void toggle_editing(){
      if (editing == 0){
        confirm_editing();
      }
      else if (editing == 1){ 
        editing = 0; 
        exit_editing();
      }      
    } 
    void up_action(){
      if (editing == 0){ 
        int next_id = -1;
        next_id = structure[menu].up_id; 
        return next_id;
      }
      return -1;
    }
    void right_action(){
      if (editing == 1){ }
    }
    void down_action(){
      Serial.println("DOWN");
      if (editing == 0){ 
       int next_id = -1;
       next_id = structure[menu].down_id; 
       return next_id;
      }
    }
    void left_action(){
      if (editing == 0){ 
        int next_id = -1;
        next_id = structure[menu].parent_id; 
        return next_id;
      }
    }
    void select_action(){
      Serial.println("TOGGLE");
      toggle_editing();
    } 
  public:
};
class widgetFactory{
  private: 
    int widget_id = -1;
    menuWidget* widget;
  public:
    key_signal(int key){
      return widget -> key_signal(key);
    }
    set_menu(int id){
      widget -> set_menu(id);
    }
    load(int id){
      if (widget_id != id){
        widget_id = id;
        switch(widget_id){
          case 0:
            widget = new menuWidget();
            break;
          case 1:
            widget = new menuWidgetInteger();
            break;
        }
        return widget;   
      }   
    }
};


class menuController{
  private:
    boolean reload = 0;
    int next_id = 0;
    int item_id = -1;
    menuItem current_item;

    int widget_id = -1;
    widgetFactory widget_factory;
    
    void set_widget(){
        widget_id = current_item.widget_id;
        widget_factory.load(widget_id);
        widget_factory.set_menu(item_id);
    }
  public:
    int factory(int key){
      return widget_factory.key_signal(key);
    }
    
    int to_load(int id){
      if (id > -1){ return id;}
      else { return next_id;}
    };
    void load(int id){
      item_id = to_load(id);
      current_item = structure[item_id];
      set_widget();
    };      
};


pump Pompe(1);
menuController menu;

void setup(){
  lcd.begin(16, 2);
  lcd.clear();

  Serial.begin(9600);

  EEPROM.write(10,20);
  /*
  EEPROM.write(1,1);
  EEPROM.write(2,2);
  EEPROM.write(3,230);
  EEPROM.write(4,180);
  EEPROM.write(5,130);*/

  menu.load(0);
}
void loop(){
  if (arrosage == 0){
    int key = listen();
    if (key > 0){ key_pushed(key);}
  }
  if (in_menu == 0){
    bool start = need_water();
  }
  delay(200);
}

int listen(){
  int x = analogRead (lcd_key);
  if (x < 50) {
    return RIGHT_KEY;
  }
  else if (x < 150) {
    return UP_KEY;
  }
  else if (x < 300){
    return DOWN_KEY;
  }
  else if (x < 500){
    return LEFT_KEY;
  }
  else if (x < 800){
    return SELECT_KEY;
  }
  return 0;
  delay(500);
};
void key_pushed(int key){
  int action = menu.factory(key);
  if (action > -1){
    menu.load(action);
    delay(500);
  }
}
bool need_water(){
  return 0;
  };
void start_arrosage(){}
