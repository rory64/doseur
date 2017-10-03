#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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

const char* menuMessage[20] = {
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
    bool exclusive;
    int address;
    int max_val;
    bool nullable;
    bool need_confirm; 
    // Program
} menuItem;


const menuItem structure[14] =  {
  // id      title, subtitle,parent_id, up_id, down_id, select_id, widget,  exclusive,            address, max_val, nullable,  need_confirm
  /* 0   */{     0,       14,        0,     0,       0,         1,      0,          0,                  0,       0,        0,             0  },
  /* 1   */{     2,       15,        0,     1,       2,         4,      0,          0,                  0,       0,        0,             0  },
  /* 2   */{     2,        1,        0,     1,       3,         0,      0,          0,                  0,       0,        0,             0  },
  /* 3   */{     2,        3,        0,     2,       3,         0,      0,          0,                  0,       0,        0,             0  },
  /* 4   */{     1,       16,        2,     4,       7,         8,      2,          1,                  0,       0,        0,             1  }, 
  /* 5   */{     4,       16,        7,     8,       9,         8,      3,          1,                  0,      10,        0,             1  },
  /* 6   */{     5,       16,        7,     8,       9,         9,      4,          1,                  0,       4,        1,             1  },
  /* 7   */{     6,       16,        7,     8,       9,         9,      5,          1,                  0,     255,        0,             1  },
  //WATER SETTINGS
  /* 8   */{     7,       10,        1,     4,       5,         4,      1,          0,WATER_PUMP_PIN     ,       4,        0,             1  }, 
  /* 9   */{     8,       10,        1,     4,       6,         5,      1,          0,WATER_QTY_PIN      ,       5,        0,             1  },
  /* 10  */{     9,       10,        1,     5,       6,         6,      1,          0,SENSOR_DELAY_PIN   ,     255,        0,             1  },
  /* 11  */{    12,       11,        3,     2,       5,         4,      1,          0,                  2,       0,        0,             0  },
  /* 12  */{    13,       12,        3,     1,       4,         6,      5,          0,                  2,       0,        0,             0  }
};
    
class menuWidget{
  protected:
    bool widget_on = 0;
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
    void set_title(char text[16]){
      lcd.setCursor(0,0);
      lcd.print(fill_line(text));
    }
    void set_subtitle(char text[16]){
        lcd.setCursor(0,1);
        lcd.print(fill_line(text));
    }
  private:
    int target = 0;

    void up_action(){
      reload = 1;
      target = current_item.up_id; 
    }
    void right_action(){
    }
    void down_action(){
      reload = 1;
      target = current_item.down_id; 
    }
    void left_action(){
      reload = 1;
      target = current_item.parent_id;
    }
    void select_action(){
      reload = 1;
      target = current_item.select_id; 
    } 
  public:
    bool reload = 0;
    menuItem current_item;
    
    void init(menuItem item){
      current_item = item;
      bool reload = 0;
      int target = 0;
      set_title(menuMessage[current_item.title]);
      set_subtitle(menuMessage[current_item.subtitle]);
    };
    int need_reload(){
      return reload;
    };
    int get_target(){
      return target;
    };
    void key_signal(int key){
      switch(key){
        case 1:
          up_action();
        case 2:
          right_action();
          break;
        case 3:
          down_action();
          break;
        case 4:
          left_action();
          break;
        case 5:
          select_action();
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
      if (editing == 1){ }
    }
    void right_action(){
      if (editing == 1){ }
    }
    void down_action(){
        if (editing == 0){ }
      }
    void left_action(){
      if (editing == 1){ }
    }
    void select_action(){
      /*if (current_item.action == 0){
        load(current_item.select_id);        
      }
      else {
        // Entree / Sortie de l'édition
        if (editing == 0){
          if (current_item.need_confirm == 1 ){ toggle_editing();} 
          else if (current_item.need_confirm == 0){ function_factory();}
          else if (editing == 0 && current_item.exclusive == 1){ function_factory();}
        }
        else {
          toggle_editing();
        }
      }  */  
    } 
  public:
};
class widgetFactory{
  private: 
    int widget_id = -1;
    menuWidget* widget;
  public:
    load(menuItem item){
      if (widget_id == item.widget_id){
        widget -> init(item);
      }
      else{
        Serial.println(item.title);
        widget_id = item.widget_id;
        switch(widget_id){
        case 0:
          widget = new menuWidget();
          break;
        case 1:
          widget = new menuWidgetInteger();
          break;
        }      
        widget -> init(item);
      }
    }
};

class menu_controller{
  private: 
    int lcd_key     = 0;
  
    widgetFactory widget_factory;
    menuWidget* widget;
    int current_id;
    int choice;
    
    void start_menu(){ select_menu();};
    
    void up(){ choice = 1;};
    void right(){ choice = 2;};
    void down(){ choice = 3;};
    void left(){ choice = 4;};
    void select(){ choice = 5;};
    
    void select_menu(int menu_id = 0){
      current_id = menu_id;
      widget = widget_factory.load(structure[menu_id]);
    }
    
    void do_action(){
      Serial.println(widget -> need_reload());
        if (widget -> need_reload() == 1){
          select_menu(widget -> get_target());
        }
        else if (choice != 0){ 
          widget->key_signal(choice);
        }
    };
  public:
    void init(){
      start_menu();
    }
    void listen(){
      choice = 0;
      // always refresh (related field)
       int x = analogRead (0);
      //Check analog values from LCD Keypad Shield
      if (x < 50) {
        right();
      }
      else if (x < 150) {
        up();
      }
      else if (x < 300){
        down();
      }
      else if (x < 500){
        left();
      }
      else if (x < 800){
       select();
      }
      do_action();
      delay(500);
    };
};


pump Pompe(1);
menu_controller menu;

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

  menu.init();
}
void loop(){
  if (arrosage == 0){
    menu.listen();
  }
  if (in_menu == 0){
    bool start = need_water();
  }
  delay(200);
}

bool need_water(){
  return 0;
  };
void start_arrosage(){}
