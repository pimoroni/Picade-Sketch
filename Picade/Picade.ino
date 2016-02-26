#include "picade.h"
#include "TimerOne.h"
#include <EEPROM.h>
#include <Keyboard.h>
#include "joystick.h"

/* Comment out this line if you want to bind the volume buttons to keys */
#define ENABLE_VOLUME_BUTTONS

/* Change this to tweak your button debounce delay. Generally the lower you can
 *  get it without seeing any unwanted repeat button presses, the better.
 */
#define DEBOUNCE_DELAY 25

const uint8_t picade_pins[] = {UP, DOWN, LEFT, RIGHT, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, START, COIN, ENTER, ESCAPE, VOL_UP, VOL_DN, MOSI, MISO, SCLK};

const uint8_t quickbind_keyboard[] = {KEY_UP_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,KEY_RIGHT_ARROW,KEY_LEFT_CTRL,KEY_LEFT_ALT,' ',KEY_LEFT_SHIFT,'z','x','s','c',KEY_RETURN,KEY_ESC,ACT_VOL_UP,ACT_VOL_DN,'i','o','p'};

const uint8_t quickbind_gamepad[] = {JOYSTICK_Y_NEG,JOYSTICK_Y_POS,JOYSTICK_X_NEG,JOYSTICK_X_POS,
    JOYSTICK_BUTTON_START,
    JOYSTICK_BUTTON_START+1,
    JOYSTICK_BUTTON_START+2,
    JOYSTICK_BUTTON_START+3,
    JOYSTICK_BUTTON_START+4,
    JOYSTICK_BUTTON_START+5,
    JOYSTICK_BUTTON_START+6,
    JOYSTICK_BUTTON_START+7,
    JOYSTICK_BUTTON_START+8,
    JOYSTICK_BUTTON_START+9,
    JOYSTICK_BUTTON_START+10,
    JOYSTICK_BUTTON_START+11,
    JOYSTICK_BUTTON_START+12,
    JOYSTICK_BUTTON_START+13,
    JOYSTICK_BUTTON_START+14};

/* Change key bindings below */
picade_config default_config = {
  CONFIG_VERSION,
  {
    {KEY_UP_ARROW},
    {KEY_DOWN_ARROW},
    {KEY_LEFT_ARROW},
    {KEY_RIGHT_ARROW},
    {KEY_LEFT_CTRL},
    {KEY_LEFT_ALT},
    {' '},
    {KEY_LEFT_SHIFT},
    {'z'},
    {'x'},
    {'s'},
    {'c'},
    {KEY_RETURN},
    {KEY_ESC},
    {'u'},
    {'d'},
    {'i'},
    {'o'},
    {'p'}
  },
  VOL_DEFAULT
};

uint8_t shift_state = 0;

static button_state button_states[PICADE_BUTTON_COUNT] = {};

/* You can ignore everything from this point onwards... */

void load_config(){

  if(valid_config()){

    Serial.println(F("Loading Config!"));
    
    EEPROM_read(CONFIG_START, default_config);
    
  }
  else
  {
      Serial.println(F("Saving Presets!"));
      save_config();
  }
}

void save_config(){
  Serial.println(F("Saving Config!"));
  
  EEPROM_write(CONFIG_START, default_config);
}

void setup() {

  Serial.begin(9600);
  //while(!Serial);
  Serial.setTimeout(100);

  load_config();
  
  Joystick.begin();
  
  for(uint8_t i = 0; i < PICADE_BUTTON_COUNT; i++){
    pinMode(picade_pins[i], INPUT_PULLUP);
  }
  pinMode(BLINK_LED, OUTPUT);
  
  volume_init();
  
  load_volume(default_config.volume);

  Timer1.initialize(5000);
  Timer1.attachInterrupt(update);
  Timer1.start();
}

void update(void){
  bool changed = false;
  // loop through each input
  for(int i = 0; i < PICADE_BUTTON_COUNT; i++)
  {
    // test for current state of this input
    boolean state = !digitalRead(picade_pins[i]);

    if(state != button_states[i].state && (millis() - button_states[i].last_change) > DEBOUNCE_DELAY) // has this input changed state since the last time we checked?
    {
      button_states[i].state = state; // update our state map so we know what's happening with this key in future
      button_states[i].last_change = millis();
      
      if(handle_key(i, state)){changed=true;}

      /*Serial.print("Event: ");
      Serial.print(state);
      Serial.print(" idx: ");
      Serial.print(i);
      Serial.print(" key: ");
      Serial.print(default_config.buttons[i]);
      Serial.println("");*/
    }
  }
  
  if(changed) Joystick.sendReport();
}

bool handle_key(uint8_t i, bool state){
  uint8_t key = default_config.buttons[i].key;

  // If shift state ALT_1 is enabled, use the alt_1 keybind
  // The key with ACT_ALT_1 bound can not be shifted!
  if(key != ACT_ALT_1 && bitRead(shift_state, BIT_ALT_1) && default_config.buttons[i].alt1 > 0){
    key = default_config.buttons[i].alt1;
  }

  // If shift state ALT_2 is enabled, use the alt_2 keybind
  // The key with ACT_ALT_2 bound can not be shifted!
  if(key != ACT_ALT_2 && bitRead(shift_state, BIT_ALT_2) && default_config.buttons[i].alt2 > 0){
    key = default_config.buttons[i].alt2;
  }
  
  // send the appropriate event
  if(key == ACT_VOL_UP && state){
        volume_target_up();
        default_config.volume = volume_target;
        return false;
  }
  if(key == ACT_VOL_DN && state){
        volume_target_down();
        default_config.volume = volume_target;
        return false;
  }
  if(key == ACT_ALT_1){
    if(bitRead(default_config.shift, BIT_MODE_1)){ // Toggle mode!
        if(state){
          bitWrite(shift_state, BIT_ALT_1, !bitRead(shift_state, BIT_ALT_1));
        }
    }
    else
    {
       bitWrite(shift_state, BIT_ALT_1,  state);
    }
  }
  if(key == ACT_ALT_2){
    if(bitRead(default_config.shift, BIT_MODE_2)){
        if(state){
          bitWrite(shift_state, BIT_ALT_2, !bitRead(shift_state, BIT_ALT_2));
        }
    }
    else
    {
       bitWrite(shift_state, BIT_ALT_2, state);
    }
  }
  
  if(key >= JOYSTICK_START){
    // Joystick Axis or Button
    switch(key){
      case JOYSTICK_X_POS:
        Joystick.setXAxis(127 * state);
        return true;
      case JOYSTICK_X_NEG:
        Joystick.setXAxis(-127 * state);
        return true;
      case JOYSTICK_Y_POS:
        Joystick.setYAxis(127 * state);
        return true;
      case JOYSTICK_Y_NEG:
        Joystick.setYAxis(-127 * state);
        return true;
      case JOYSTICK_Z_POS:
        Joystick.setZAxis(127 * state);
        return true;
      case JOYSTICK_Z_NEG:
        Joystick.setZAxis(-127 * state);
        return true;
      default:
        if( key >= JOYSTICK_BUTTON_START ){
          uint8_t button = key - JOYSTICK_BUTTON_START;
          Joystick.setButton(button, state);
          return true;
        }
        return false;
    }
  }
  
  if(key > 12){
    // Keyboard Button
    if(state){ Keyboard.press(key);}else{Keyboard.release(key);}
    return false;
  }
}


void loop() {
  uint8_t index;
  uint8_t keycode;
  char spacer;

  while(Serial.available()){
    uint8_t cmd = Serial.read();

    switch(cmd){
      case 'v':
        Serial.println(F(CONFIG_VERSION));
        break;
      case 'h':
        picade_help();
        break;
      case 'm': // Change Alt1/Alt2 modifier options
          Serial.read(); // Throw away separator char
          index = Serial.parseInt();
          Serial.read(); // Throw away separator char
          keycode = Serial.read(); // Read config char ( h or t )
          if(index > 0 && (keycode == 'h' || keycode == 't')){
            
            if(index == 1){
              if(keycode == 'h'){
                bitWrite(default_config.shift, BIT_MODE_1, 0);
                Serial.println(F("Alt1 set to hold"));
              }
              else
              {
                bitWrite(default_config.shift, BIT_MODE_1, 1);
                Serial.println(F("Alt1 set to toggle"));
              }
            }
            if(index == 2){
              if(keycode == 'h'){
                bitWrite(default_config.shift, BIT_MODE_2, 0);
                Serial.println(F("Alt2 set to hold"));
              }
              else
              {
                bitWrite(default_config.shift, BIT_MODE_2, 1);
                Serial.println(F("Alt2 set to toggle"));
              }
            }
          }
          else
          {
            Serial.println(F("Usage: m <1 alt1, 2 alt2> <h hold, t toggle>"));
            Serial.println(F("EG: m 1 t  =  Set alt1 to (t)oggle mode"));
          }
          break;
      case 'k': // Quick Keyboard Bind
        for( int x = 0; x < PICADE_BUTTON_COUNT; x++){
          default_config.buttons[x].key = quickbind_keyboard[x];
        }
        Serial.println(F("Loaded Keyboard"));
        break;
      case 'g': // Quick Gamepad Bind
        for( int x = 0; x < PICADE_BUTTON_COUNT; x++){
          default_config.buttons[x].key = quickbind_gamepad[x];
        }
        Serial.println(F("Loaded Gamepad"));
        break;
      case 'd':
        Serial.print('a');
        for( int x = 0; x < PICADE_BUTTON_COUNT; x++){
          Serial.print(" ");Serial.print(default_config.buttons[x].key);
        }
        for( int x = 0; x < PICADE_BUTTON_COUNT; x++){
          Serial.print(" ");Serial.print(default_config.buttons[x].alt1);
        }
        for( int x = 0; x < PICADE_BUTTON_COUNT; x++){
          Serial.print(" ");Serial.print(default_config.buttons[x].alt2);
        }
        Serial.println();
        break;
      case 'a':
        index = 0;
        while(1){
          spacer = Serial.read(); // Throw away separator char
          if(spacer != ' '){break;} // Timeout
          keycode = Serial.parseInt();
          if(index >= PICADE_BUTTON_COUNT * 2){
            default_config.buttons[index-(PICADE_BUTTON_COUNT*2)].alt2 = keycode;
          }
          else if(index >= PICADE_BUTTON_COUNT){
            default_config.buttons[index-PICADE_BUTTON_COUNT].alt1 = keycode;
          }
          else {
            default_config.buttons[index].key = keycode;
          }
          index++;
        }
        if(index > 0){
          Serial.print(F("Bound "));Serial.print(index);Serial.println(F(" key(s)"));
        }
        else
        {
          Serial.println(F("Usage: a <keycode> <keycode> ..."));
        }
        break;
      case 'b':
        Serial.read();
        index = Serial.parseInt();
        if( index < PICADE_BUTTON_COUNT ){
          
          Serial.read(); // Drop Spacer
          keycode = Serial.parseInt();
          if(keycode > 0){
            handle_key(default_config.buttons[index].key, false);
            default_config.buttons[index].key = keycode;
            
            Serial.print(F("Bound: "));
            Serial.print(index);
            Serial.print(F(" to: "));
            Serial.print(keycode);
          
            if(Serial.read() == ' '){ // Read next spacer
              keycode = Serial.parseInt();
              handle_key(default_config.buttons[index].alt1, false);
              default_config.buttons[index].alt1 = keycode;
              
              Serial.print(F(" alt1: "));
              Serial.print(keycode);
  
              if(Serial.read() == ' '){ // Read next spacer
                keycode = Serial.parseInt();
                handle_key(default_config.buttons[index].alt2, false);
                default_config.buttons[index].alt2 = keycode;
              
                Serial.print(F(" alt2: "));
                Serial.print(keycode);
                
              }
            }
            
            Serial.println();
          }
          else
          {
            Serial.println(F("Usage: b <button> <keycode> <optional:alt1> <optional:alt2>"));
          }
          
        }
        break;
      case 's':
        save_config();
        break;
      case 'l':
        load_config();
        break;
      case '+':
        volume_target_up();
        default_config.volume = volume_target;
        Serial.print(F("Volume: "));
        Serial.println(volume_target);
        break;
      case '-':
        volume_target_down();
        default_config.volume = volume_target;
        Serial.print(F("Volume: "));
        Serial.println(volume_target);
        break;
    }
  }

#ifdef ENABLE_VOLUME_BUTTONS
 /*
  * Read the current states of the Volume Up, Volume Down and Headphone Detect inputs
  */
  boolean headphone = !digitalRead(HEADPHONE_DETECT);

  /*
   * If headphones are unpugged/plugged in then adjust the volume accordingly.
   * Save the target volume and set it to 0 to fade out.
   * Load the saved value into the target volume to fade back in.
   */
  if(headphone != last_headphone){
    last_headphone = headphone;
    if(headphone){
      volume_target_save();
      volume_target = 0;
    }
    else
    {
      volume_target_load();
    }
  }

  volume_track();
#endif

}
