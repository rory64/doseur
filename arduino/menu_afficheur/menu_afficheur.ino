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
#define COMPONENT_START_PIN       270 // 18 addresses by component
#define COMPONENT_WORD_LENGTH     18 // 18 addresses by component

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

  EEPROM.write(COMPONENT_START_PIN, 66);
  EEPROM.write(COMPONENT_START_PIN + 1, 73);
  EEPROM.write(COMPONENT_START_PIN + 2, 79);
  EEPROM.write(COMPONENT_START_PIN + 3, 70);
  EEPROM.write(COMPONENT_START_PIN + 4, 73);
  EEPROM.write(COMPONENT_START_PIN + 5, 83);
  EEPROM.write(COMPONENT_START_PIN + 6, 72);
  EEPROM.write(COMPONENT_START_PIN + 7, 32);
  EEPROM.write(COMPONENT_START_PIN + 8, 32);
  EEPROM.write(COMPONENT_START_PIN + 9, 32);
  EEPROM.write(COMPONENT_START_PIN + 10, 32);
  EEPROM.write(COMPONENT_START_PIN + 11, 32);
  EEPROM.write(COMPONENT_START_PIN + 12, 32);
  EEPROM.write(COMPONENT_START_PIN + 13, 32);
  EEPROM.write(COMPONENT_START_PIN + 14, 32);
  EEPROM.write(COMPONENT_START_PIN + 15, 32);
  EEPROM.write(COMPONENT_START_PIN + 18, 70);
  EEPROM.write(COMPONENT_START_PIN + 19, 79);
  EEPROM.write(COMPONENT_START_PIN + 20, 70);
  EEPROM.write(COMPONENT_START_PIN + 21, 79);
  EEPROM.write(COMPONENT_START_PIN + 22, 73);
  EEPROM.write(COMPONENT_START_PIN + 23, 72);
  EEPROM.write(COMPONENT_START_PIN + 24, 32);
  EEPROM.write(COMPONENT_START_PIN + 25, 32);
  EEPROM.write(COMPONENT_START_PIN + 26, 32);
  EEPROM.write(COMPONENT_START_PIN + 27, 32);
  EEPROM.write(COMPONENT_START_PIN + 28, 32);
  EEPROM.write(COMPONENT_START_PIN + 29, 32);
  EEPROM.write(COMPONENT_START_PIN + 30, 32);
  EEPROM.write(COMPONENT_START_PIN + 31, 32);
  EEPROM.write(COMPONENT_START_PIN + 32, 32);
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
    };
    int add(int todo_qty){
      qty = todo_qty;
      calculate_values();
      process();
    };
  };

class preparation{};

// ------------------------------
// ----- MENU SECTION START -----
// ------------------------------
int row = 0;
int col = 0;

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
  /* 2   REGLAGES COMPOSANTS */{     2,        1,        0,     1,       3,         4,      2,                  0,       0,        0  },
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

class menu{
  protected:
    int id;
    int next_id = -1;
    void set_title(char text[16], int center=1){
      row = 0;
      col = 0;
      print_line(text, center);
    }
    void set_subtitle(char text[16], int center=1){
      row = 1;
      col = 0;
      print_line(text, center);
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
  private:    
    void print_line(char text[16], int center=1){
      lcd.setCursor(col, row);
      if (center == 0){ lcd.print(text);}
      if (center == 1){ lcd.print(center_line(text));}      
    }
    String center_line(char text[16]){
      int len = String(text).length();
      String message;
      if (message.length() < 16){
        while (message.length() < (16 - len) / 2){
          message.concat(" ");
        }
        message.concat(text);
        while (message.length() < 16){
          message.concat(" ");
        }   
      }  
      else { message.concat(text);}
      return message; 
    }
    void confirm_editing(){
      editing = 1;
      Serial.print("<< EDITING ACTIVATED >>");
    }
    void exit_editing(){
      editing = 0;      
      Serial.print("<< EDITING STOPPED >>");
    }
  public:
    int editing = 0;
};

class menuWidget: public menu{
  private:    
    int up_action(){
      next_id = structure[id].up_id; 
      return next_id;
    }
    int right_action(){
      return -1;
    }
    int down_action(){
      next_id = structure[id].down_id; 
      return next_id;
    }
    int left_action(){
      next_id = structure[id].parent_id; 
      return next_id;
    }
    int select_action(){
      next_id = structure[id].select_id; 
      return next_id;
    } 
    
  public:
    void set_menu(int menu_id){
      Serial.print("BEGIN SIMPLE WIDGET - MENU ID : ");
      Serial.println(menu_id);
      id = menu_id;
      set_title(messageFactory[structure[id].title], 1);
      set_subtitle(messageFactory[structure[id].subtitle], 1);
    }
    int key_signal(int key){
      if (key == -1){ return -1; }
      else {
        switch(key){
          case UP_KEY:
            return up_action();
            break;
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
    }
};
class menuWidgetInteger: public menu{
  private:
    int max_val = 255;
    bool nullable = 0;
    // integer ml : update qty
    // Refresh value after update
    void update_eeprom_value(int ml=0){
      Serial.println("Update EEPROM Value");
      int val = EEPROM.read(structure[id].address);
      if (1 - structure[id].nullable <= val + ml &&  val + ml <= structure[id].max_val) {
        EEPROM.write(structure[id].address, val + ml); 
      }
      show_integer_widget();
    }
    void show_integer_widget(){
      Serial.println("Start show widget");
        int val = EEPROM.read(structure[id].address);
        String val_str = String(val, DEC);
        int val_len = val_str.length();
        
        String message;
        if (val > 1 - structure[id].nullable){ message.concat("-");}
        else { message.concat(" ");}
        message.concat("     ");
        if (val_len < 4){ message.concat(" ");}
        if (val_len == 1){ message.concat(" ");}
        message.concat(val);
        if (val_len == 2){ message.concat(" ");}
        if (val_len == 1){ message.concat(" ");}
        message.concat("     ");
        if (structure[id].max_val == val){ message.concat(" ");}
        else { message.concat("+");}

        char text[17];
        message.toCharArray(text,17);
        set_subtitle(text);
        Serial.println("Stop show widget");
    }
    int up_action(){
      if (editing == 0){ 
        next_id = structure[id].up_id; 
        return next_id;
      }
      return -1;
    }
    int right_action(){
      if (editing == 1){
        update_eeprom_value(1);
        return -1;
      }
    }
    int down_action(){
      if (editing == 0){ 
       next_id = structure[id].down_id; 
       return next_id;
      }
      else { return -1; }
    }
    int left_action(){
      if (editing == 1){
        update_eeprom_value(-1);
        return -1;
      }
      else {        
        next_id = structure[id].parent_id; 
        return next_id;
      }
    }
    int select_action(){
      toggle_editing();
      if (editing == 0){ return id; }
      else {
        show_integer_widget();
        return -1;
      }
    }
  public:
    void set_menu(int menu_id){
      Serial.print("BEGIN INTEGER WIDGET - MENU ID : ");
      Serial.println(menu_id);
      id = menu_id;
      set_title(messageFactory[structure[id].title]);
      set_subtitle(messageFactory[structure[id].subtitle]);
    }
    int key_signal(int key, int edit){
      editing = edit;
      switch(key){
        case UP_KEY:
          return up_action();
          break;
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
class menuWidgetComponent: public menu{
  private:
    int item_id;
    char item_name[16];
    int mode;

    void action_factory(){
      switch(mode){
        case 0:
          list();
          break;
        case 1:
          action_create();
          break;
        case 2:
          action_read();
          break;
        case 3:
          action_update();
          break;
        case 4:
          action_delete();
          break;
      }
    }

    void get_name(){
      String result;
      int count = 0;
      int start = COMPONENT_START_PIN + (COMPONENT_WORD_LENGTH * item_id);
      
      while(count < 16){
        result.concat(char(EEPROM.read(start + count)));
        count += 1;
      }
      result.toCharArray(item_name, 16);
    }

    void list(){
      col = 0;
      row = 0;

      get_name();
      set_title(item_name, 0);
    }
    void set_next_item(){
      item_id += 1;
    }
    void set_previous_item(){
      item_id -= 1;
    }
    
    void action_create(){}
    void action_read(){}
    void action_update(){}
    void action_delete(){}
    
    int up_action(){
      if (editing == 0){ 
       next_id = structure[id].down_id; 
       return next_id;
      }
      else { 
        set_previous_item();
        action_factory();
        return -1; 
      }
    }
    int right_action(){
      if (editing == 1){
        return -1;
      }
    }
    int down_action(){
      if (editing == 0){ 
       next_id = structure[id].down_id; 
       return next_id;
      }
      else { 
        set_next_item();
        action_factory();
        return -1; 
      }
    }
    int left_action(){
      if (editing == 1){
        return -1;
      }
      else {        
        next_id = structure[id].parent_id; 
        return next_id;
      }
    }
    int select_action(){
      toggle_editing();
      if (editing == 0){ return id; }
      else {
        list();
        return -1;
      }
    }
  public:
    int get_item(){
      return item_id;
    }
    int get_mode(){
      return mode;
    }
    void set_menu(int menu_id){
      Serial.print("BEGIN CRUD WIDGET - MENU ID : ");
      Serial.println(menu_id);
      id = menu_id;
      item_id = 0;
      mode = 0;
      set_title(messageFactory[structure[id].title], 1);
      set_subtitle(messageFactory[structure[id].subtitle], 1);      
    }
    int key_signal(int key, int _edit, int _item_id, int _mode){
      editing = _edit;
      item_id = _item_id;
      mode = _mode;
      switch(key){
        case UP_KEY:
          return up_action();
          break;
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
      
      Serial.print("COMPONENT ID : ");
      Serial.println(item_id);
      Serial.println("-------------------------");
      Serial.println(" ");
    }
};
class widgetFactory{
  private:
    int editing = 0;
    int item_id = 0;
    int mode = 0;
  public:
    load(int menu_id, int key){
      Serial.print("KEY SIGNAL RECEIVED : ");
      Serial.println(key);
      menuItem menu;
      menu = structure[menu_id];
      switch(menu.widget_id){
        case 0:
          {menuWidget simpleMenu;
          simpleMenu.set_menu(menu_id);
          if (key > -1){ return simpleMenu.key_signal(key); }
          else { return -1; }
          break;}
        case 1:
          {menuWidgetInteger integerMenu;
          integerMenu.set_menu(menu_id);
          if (key > -1){ 
            int response = integerMenu.key_signal(key, editing);
            editing = integerMenu.editing;
            return response;
          }
          else { return -1; }
          break;}
        case 2:
          {menuWidgetComponent component;
          component.set_menu(menu_id);
          if (key > -1){ 
      Serial.println(editing);
            int response = component.key_signal(key, editing, item_id, mode);
            editing = component.editing;
      Serial.println(editing);
            item_id = component.get_item();
            mode = component.get_mode();
            return response;
          }
          else { return -1; }
          break;}
        }
     }
};

class menuController{
  private:
    int item_id = -1;
    int widget_id = -1;

    menuItem menu;
    
    widgetFactory widget_factory;
    
    int load_menu(int key){
      if (item_id > -1){ return widget_factory.load(item_id, key);}
    }
  public:
    int factory(int key){
      return load_menu(key);
    }
    void load(int id){
      item_id = id;
      menu = structure[id];
      load_menu(-1);
      Serial.println("--------------------------");
      Serial.println(" ");
    };      
};

menuController menu;
// ------------------------------
// ------ MENU SECTION STOP -----
// ------------------------------

pump Pompe(1);

void setup(){
  lcd.begin(16, 2);
  lcd.clear();

  Serial.begin(9600);

  usine();

  /*
  EEPROM.write(10,20);
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
  int response = menu.factory(key);
  Serial.print("WIDGET RESPONSE : ");
  Serial.println(response);
  if (response > -1){
    menu.load(response);
    delay(500);
  }
}

bool need_water(){
  return 0;
  };

void start_arrosage(){}
