#include <arduino.h>
#include <EEPROM.h>
#include <Keyboard.h>

/*
 * There are 32 discrete gain settings from +24dB maximum to -80dB minimum
 * On power up the gain is set to a default of 12dB
 */
#define VOL_SAVE_FREQ 2000 // Save frequency in milliseconds
#define VOL_DELAY  40
#define VOL_DEFAULT 20
#define VOL_MAX 24

#define CONFIG_START 8
#define PICADE_BUTTON_COUNT 25

#define GPIO_1     PD6
#define GPIO_2     PD7
#define GPIO_3     PB4
#define GPIO_4     PB5
#define GPIO_5     PB6
#define GPIO_6     PC6

#define BTN_1      PF7  // PF0  ADC0
#define BTN_2      PF6  // PF1  ADC1
#define BTN_3      PF5  // PF4  ADC4
#define BTN_4      PF4  // PF5  ADC5
#define BTN_5      PF1  // PF6  ADC6
#define BTN_6      PF0  // PF7  ADC7

// DigitalRead understands Analog Pin names, but AnalogRead doesn't understand digital pin names =/ At least not in the case of AnalogRead(6)
#define LEFT       PB3  // PB6  ADC13
#define RIGHT      PB2  // PB5  ADC12
#define UP         PB0  // PD7  ADC10
#define DOWN       PB1  // PB4  ADC11

/* System Controls */

#define START      PD0  // PD6  ADC9
#define COIN       PD1  // PD4  ADC8   (1/4) on silk
#define ESCAPE     PD2  // PD3/TX
#define ENTER      PD3  // PD2/RX    1UP on silk

/* Audio & Amplifier */

#define VOL_UP      3  // PD0
#define VOL_DN      2  // PD1

#define HEADPHONE_DETECT 5  // PC6

#define AMP_UP      11 // PB7
#define AMP_DN       7 // PB6

#define BLINK_LED   13 // PC7

#define MISO 14
#define SCLK 15
#define MOSI 16

#define JOY1_X 246
#define JOY1_Y 247
#define JOY2_X 248
#define JOY2_Y 249

#define ACT_VOL_UP 250
#define ACT_VOL_DN 251

#define ACT_ALT_1 252
#define ACT_ALT_2 253

#define JOYSTICK_START 220
#define JOYSTICK_X_POS 223 // Right
#define JOYSTICK_X_NEG 222 // Left
#define JOYSTICK_Y_POS 221 // Down
#define JOYSTICK_Y_NEG 220 // Up

#define JOYSTICK_Z_POS 224
#define JOYSTICK_Z_NEG 225

#define JOYSTICK_BUTTON_START 230

#define CONFIG_VERSION "Picade v2.3"
#define CONFIG_VERSION_SIZE 12

#define BIT_ALT_1 0
#define BIT_ALT_2 1
#define BIT_MODE_1 2
#define BIT_MODE_2 3

bool valid_config(){
  for(uint8_t x = 0; x < CONFIG_VERSION_SIZE; x++){
    if(EEPROM.read(CONFIG_START + x) != CONFIG_VERSION[x]) return false;
  }
  return true;
}

template <class T> int EEPROM_write(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.update(ee++, *p++);
    return i;
}

template <class T> int EEPROM_read(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

typedef struct
{
  uint8_t key;
  uint8_t alt1;
  uint8_t alt2;
} key_bind;

typedef struct
{
  uint8_t state;    // what state was the input last in ( HIGH/LOW )
  unsigned long last_change;
} button_state;

typedef struct
{
  char version[CONFIG_VERSION_SIZE];
  key_bind buttons[PICADE_BUTTON_COUNT];
  uint8_t shift;
  uint8_t volume;
} picade_config;

volatile int volume_current  = 0;
int volume_target   = VOL_MAX;
int volume_saved    = 0;
int volume_eeprom   = 0;

unsigned long last_vol_up_time = 0;
unsigned long last_vol_dn_time = 0;

boolean last_vol_up    = 0;
boolean last_vol_dn    = 0;
boolean last_headphone = 1;

unsigned long last_headphone_time = 0;

void volume_target_save(){
  volume_saved = volume_target;
}

void volume_target_load(){
  volume_target = volume_saved;
  if( volume_target > VOL_MAX ) volume_target = VOL_MAX;
}

int volume_target_reached(){
  return (volume_target == volume_current);
}

void headphone_detect(){
  /*
     Read the current states of the Volume Up, Volume Down and Headphone Detect inputs
  */
  boolean headphone = !digitalRead(HEADPHONE_DETECT);

  /*
     If headphones are unpugged/plugged in then adjust the volume accordingly.
     Save the target volume and set it to 0 to fade out.
     Load the saved value into the target volume to fade back in.
  */
  if (headphone != last_headphone){
    delay(25);
     if((!digitalRead(HEADPHONE_DETECT)) == headphone){
      if(millis() - last_headphone_time > 1000) {
        last_headphone = headphone;
        last_headphone_time = millis();
        if (headphone) {
          Serial.println(F("Headphones Detected, Vol: 0"));
          volume_target_save();
          volume_target = 0;
        }
        else
        {
          volume_target_load();
          Serial.print(F("Headphones Removed, Vol:"));
          Serial.println(volume_target);
        }
      }
    }
  }
}

void volume_up(){
  if(volume_current == VOL_MAX) return;
  volume_current++;
  digitalWrite(AMP_UP, LOW);
  delay(VOL_DELAY);
  digitalWrite(AMP_UP, HIGH);
  delay(5);
};

void volume_down(){ 
  if(volume_current == 0) return;
  volume_current--;
  digitalWrite(AMP_DN, LOW);
  delay(VOL_DELAY);
  digitalWrite(AMP_DN, HIGH);
  delay(5);
};

void volume_init(){
  pinMode(AMP_UP, OUTPUT);
  digitalWrite(AMP_UP, HIGH);
  pinMode(AMP_DN, OUTPUT);
  digitalWrite(AMP_DN, HIGH);

  pinMode(HEADPHONE_DETECT, INPUT);

  last_headphone = !digitalRead(HEADPHONE_DETECT);
}

void volume_track(){
  if(volume_target > volume_current){
    volume_up();
  }
  
  if(volume_target < volume_current){
    volume_down();
  }
}

void volume_fade(int level){

  if(level > volume_current){
    while(level > volume_current){
      volume_up();
    }
  }
  else
  {
    while(level < volume_current){
      volume_down();
    }
  }
  
}

void volume_target_up(){
  if(volume_target == VOL_MAX) return;
  volume_target++;
}

void volume_target_down(){
  if(volume_target == 0) return;
  volume_target--;
}

void volume_reset(){
  volume_current=34;
  while(volume_current > 0){
    volume_down();
  }
}

void volume_set(uint8_t volume){
  volume_reset();
  volume_target = volume;
}


void picade_help(){
  
        Serial.println(F("\nPicade Config"));
        Serial.println(F("Bind single:    b <button> <keycode> <optional:alt1> <optional:alt2>"));
        Serial.println(F("Bind all:       a <keycode> <keycode> ..."));
        Serial.println(F("Dump all:       d"));
        Serial.println(F("Mode:           m <1 alt1, 2 alt2> <h hold, t toggle>"));
        Serial.println(F("Save:           s"));
        Serial.println(F("Load:           l"));
        Serial.println(F("Quick Keyboard: k"));
        Serial.println(F("Quick Gamepad:  g"));

        Serial.println(F("\n<button> table:\n"));

        Serial.print(0);Serial.print(F("  = Joy U    "));
        Serial.print(1);Serial.print(F("  = Joy D    "));
        Serial.print(2);Serial.print(F("  = Joy L    "));
        Serial.print(3);Serial.print(F("  = Joy R    "));

        Serial.println();
        
        Serial.print(4);Serial.print(F("  = Btn 1    "));
        Serial.print(5);Serial.print(F("  = Btn 2    "));
        Serial.print(6);Serial.print(F("  = Btn 3    "));
        Serial.print(7);Serial.print(F("  = Btn 4    "));
        Serial.print(8);Serial.print(F("  = Btn 5    "));
        Serial.print(9);Serial.print(F("  = Btn 6    "));

        Serial.println();
        
        Serial.print(10);Serial.print(F(" = Start    "));
        Serial.print(11);Serial.print(F(" = Coin     "));
        Serial.print(12);Serial.print(F(" = Enter    "));
        Serial.print(13);Serial.print(F(" = Escape   "));

        Serial.println();
        
        Serial.print(14);Serial.print(F(" = Vol Up   "));
        Serial.print(15);Serial.print(F(" = Vol Down "));

        Serial.println();
        
        Serial.print(16);Serial.print(F(" = MOSI     "));
        Serial.print(17);Serial.print(F(" = MISO     "));
        Serial.print(18);Serial.print(F(" = SCLK     "));

        Serial.println();

        Serial.println(F("\n<keycode> table:\n"));


        Serial.print(JOYSTICK_Y_NEG);Serial.print(F(" = Gpad U   "));
        Serial.print(JOYSTICK_Y_POS);Serial.print(F(" = Gpad D   "));
        Serial.print(JOYSTICK_X_NEG);Serial.print(F(" = Gpad L   "));
        Serial.print(JOYSTICK_X_POS);Serial.print(F(" = Gpad R   "));
        Serial.print(JOYSTICK_Z_POS);Serial.print(F(" = Gpad Z+  "));
        Serial.print(JOYSTICK_Z_NEG);Serial.print(F(" = Gpad Z-  "));

        Serial.println();

        Serial.print(JOY1_X);Serial.print(F(" = Joy 1 X  "));
        Serial.print(JOY1_Y);Serial.print(F(" = Joy 1 Y  "));
        Serial.print(JOY2_X);Serial.print(F(" = Joy 2 X  "));
        Serial.print(JOY2_Y);Serial.print(F(" = Joy 2 Y  "));
        
        Serial.println();

        for(int x = JOYSTICK_BUTTON_START; x < JOYSTICK_BUTTON_START+16; x++){
          Serial.print(x);
          Serial.print(F(" = Gpad "));
          Serial.print(x - JOYSTICK_BUTTON_START);
          if( x - JOYSTICK_BUTTON_START < 10 ) Serial.print(" ");
          Serial.print("  ");
          
          if((x-1) % 4 == 0){
            Serial.println();
          }
        }
        
        Serial.println();

        Serial.print(ACT_VOL_UP);      Serial.print(F(" = Vol +   "));
        Serial.print(ACT_VOL_DN);      Serial.print(F(" = Vol -   "));

        Serial.print(ACT_ALT_1);       Serial.print(F(" = Alt 1   "));
        Serial.print(ACT_ALT_2);       Serial.print(F(" = Alt 2   "));
        
        Serial.println();
        
        Serial.print(KEY_UP_ARROW);    Serial.print(F(" = U Arrow "));
        Serial.print(KEY_DOWN_ARROW);  Serial.print(F(" = D Arrow "));
        Serial.print(KEY_LEFT_ARROW);  Serial.print(F(" = L Arrow "));
        Serial.print(KEY_RIGHT_ARROW); Serial.print(F(" = R Arrow "));
        
        Serial.println();
        
        Serial.print(KEY_LEFT_CTRL);   Serial.print(F(" = L Ctrl  "));
        Serial.print(KEY_LEFT_SHIFT);  Serial.print(F(" = L Shift "));
        Serial.print(KEY_LEFT_ALT);    Serial.print(F(" = L Alt   "));
        Serial.print(KEY_LEFT_GUI);    Serial.print(F(" = L GUI   "));
        
        Serial.println();
        
        Serial.print(KEY_RIGHT_CTRL);  Serial.print(F(" = R Ctrl  "));
        Serial.print(KEY_RIGHT_SHIFT); Serial.print(F(" = R Shift "));
        Serial.print(KEY_RIGHT_ALT);   Serial.print(F(" = R Alt   "));
        Serial.print(KEY_RIGHT_GUI);   Serial.print(F(" = R GUI   "));
        
        Serial.println();
        
        Serial.print(KEY_BACKSPACE);   Serial.print(F(" = Backspc "));
        Serial.print(KEY_TAB);         Serial.print(F(" = TAB     "));
        Serial.print(KEY_RETURN);      Serial.print(F(" = Return  "));
        Serial.print(KEY_ESC);         Serial.print(F(" = ESC     "));
        
        Serial.println();
        
        Serial.print(KEY_INSERT);      Serial.print(F(" = Insert  "));
        Serial.print(KEY_DELETE);      Serial.print(F(" = Delete  "));
        Serial.print(KEY_PAGE_UP);     Serial.print(F(" = Pg Up   "));
        Serial.print(KEY_PAGE_DOWN);   Serial.print(F(" = Pg Down "));
        
        Serial.println();
        
        Serial.print(KEY_HOME);        Serial.print(F(" = Home    "));
        Serial.print(KEY_END);         Serial.print(F(" = End     "));
        Serial.print(KEY_CAPS_LOCK);   Serial.print(F(" = Caps Lk "));
        Serial.print(13);              Serial.print(F("  = Space   "));
        
        Serial.println();
        
        Serial.print(KEY_F1);          Serial.print(F(" = F1      "));
        Serial.print(KEY_F2);          Serial.print(F(" = F2      "));
        Serial.print(KEY_F3);          Serial.print(F(" = F3      "));
        Serial.print(KEY_F4);          Serial.print(F(" = F4      "));
        
        Serial.println();
        
        Serial.print(KEY_F5);          Serial.print(F(" = F5      "));
        Serial.print(KEY_F6);          Serial.print(F(" = F6      "));
        Serial.print(KEY_F7);          Serial.print(F(" = F7      "));
        Serial.print(KEY_F8);          Serial.print(F(" = F8      "));
        
        Serial.println();
        
        Serial.print(KEY_F9);          Serial.print(F(" = F9      "));
        Serial.print(KEY_F10);         Serial.print(F(" = F10     "));
        Serial.print(KEY_F11);         Serial.print(F(" = F11     "));
        Serial.print(KEY_F12);         Serial.print(F(" = F12     "));

        Serial.println();
        Serial.println();
        
        for(int x = 33;  x < 127; x++){
          Serial.print(x);
          if( x < 100) Serial.print(" ");
          
          Serial.print(F(" = ")); Serial.print((char)x); Serial.print(F("       "));
          if(x % 4 == 0){
            Serial.println();
          }
        }
        
        Serial.println();
}


