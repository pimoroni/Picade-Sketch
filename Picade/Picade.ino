#include "picade.h"
#include "TimerOne.h"
#include <EEPROM.h>
#include <Keyboard.h>

/* Comment out this line if you want to bind the volume buttons to keys */
#define ENABLE_VOLUME_BUTTONS

/* Change this to tweak your button debounce delay. Generally the lower you can
 *  get it without seeing any unwanted repeat button presses, the better.
 */
#define DEBOUNCE_DELAY 25

/* Change key bindings below */
static input inputs[] = {
  { KEY_UP_ARROW,    UP     },
  { KEY_DOWN_ARROW,  DOWN   },
  { KEY_LEFT_ARROW,  LEFT   },
  { KEY_RIGHT_ARROW, RIGHT  },

  { KEY_LEFT_CTRL,   BTN_1  },
  { KEY_LEFT_ALT,    BTN_2  },
  { ' ',             BTN_3  },
  { KEY_LEFT_SHIFT,  BTN_4  },
  { 'z',             BTN_5  },
  { 'x',             BTN_6  },

  { 's',             START  },
  { 'c',             COIN   },
  { KEY_RETURN,      ENTER  },
  { KEY_ESC,         ESCAPE },

#ifndef ENABLE_VOLUME_BUTTONS
  /* Change these lines to set key bindings for VOL_UP and VOL_DN */
   { 'u',      VOL_UP  },
   { 'd',      VOL_DN },
#endif
};

/* You can ignore everything from this point onwards... */

void setup() {

  //Serial.begin(9600);
  
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++){
    pinMode(inputs[i].pin, INPUT_PULLUP);
  }
  pinMode(BLINK_LED, OUTPUT);

  pinMode(HEADPHONE_DETECT, INPUT);

#ifdef ENABLE_VOLUME_BUTTONS
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DN, INPUT_PULLUP);
#endif
  
  pinMode(AMP_UP, OUTPUT);
  digitalWrite(AMP_UP, HIGH);
  pinMode(AMP_DN, OUTPUT);
  digitalWrite(AMP_DN, HIGH);

#ifdef ENABLE_VOLUME_BUTTONS
  load_volume_from_eeprom();
#endif

  Timer1.initialize(5000);
  Timer1.attachInterrupt(update);
  Timer1.start();
}

void update(void){
  // loop through each input
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++)
  {
    // test for current state of this input
    boolean state = !digitalRead(inputs[i].pin);

    if(state != inputs[i].state && (millis() - inputs[i].last_change) > DEBOUNCE_DELAY) // has this input changed state since the last time we checked?
    {
      inputs[i].state = state; // update our state map so we know what's happening with this key in future
      inputs[i].last_change = millis();
      // send the key press or release event
      if(state){ Keyboard.press(inputs[i].key);}else{Keyboard.release(inputs[i].key);}
    }
  }
}


void loop() {

#ifdef ENABLE_VOLUME_BUTTONS
 /*
  * Read the current states of the Volume Up, Volume Down and Headphone Detect inputs
  */
  boolean vol_up    = !digitalRead(VOL_UP);
  boolean vol_dn    = !digitalRead(VOL_DN);
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

  if( vol_up != last_vol_up || (millis() - last_vol_up_time) > 100 ){
    if( vol_up && volume_target < 31 ){
      volume_target++;
      last_vol_up_time = millis();
    }
    last_vol_up = vol_up;
  }

  if( vol_dn != last_vol_dn || (millis() - last_vol_dn_time) > 100 ){
    if( vol_dn && volume_target > 0 ){
      volume_target--;
      last_vol_dn_time = millis();
    }
    last_vol_dn = vol_dn;
  }

  if(volume_target > volume_current){
    volume_up();
  }
  
  if(volume_target < volume_current){
    volume_down();
  }

  /* If we've reached the target volume, save it to EEPROM and blink the notification LED */
  if(!headphone && volume_current == volume_target && volume_eeprom != volume_current && (millis() - last_vol_up_time) > 1000 && (millis() - last_vol_dn_time) > 1000){
    EEPROM.write(0, volume_current);
    volume_eeprom = volume_current;
    int x = 2;
    while(x--){
      digitalWrite(BLINK_LED, HIGH);
      delay(50);
      digitalWrite(BLINK_LED, LOW);
      delay(50);
    }
  }
#endif

}
