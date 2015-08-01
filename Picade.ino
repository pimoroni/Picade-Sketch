#include "picade.h"
#include <EEPROM.h>

static input inputs[] = {
  /*

  */
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
  { KEY_ESC,         ESCAPE }
};


void setup() {
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++){
    pinMode(inputs[i].pin, INPUT_PULLUP);
  }
  pinMode(BLINK_LED, OUTPUT);

  pinMode(HEADPHONE_DETECT, INPUT);
  
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DN, INPUT_PULLUP);
  pinMode(AMP_UP, OUTPUT);
  digitalWrite(AMP_UP, HIGH);
  pinMode(AMP_DN, OUTPUT);
  digitalWrite(AMP_DN, HIGH);

  volume_reset();
  volume_target = EEPROM.read(0);
  if(volume_target > 31){
    volume_target = 31;
  }
}


void loop() {
  /*
    Check the state of each input, if it has changed then output the corresponding keypress
  */
  boolean changed = false;  // has any input changed?

  // loop through each input
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++)
  {
    // test for current state of this input
    boolean state = !digitalRead(inputs[i].pin);

    if(state != inputs[i].state) // has this input changed state since the last time we checked?
    {
      changed = true; // something changed, we should remember that
      inputs[i].state = state; // update our state map so we know what's happening with this key in future
      
      // send the key press or release event
      if(state){ Keyboard.press(inputs[i].key);}else{Keyboard.release(inputs[i].key);}
    }
  }

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

  if( vol_up && vol_dn && vol_up != last_vol_up && vol_dn != last_vol_dn ){
      if( volume_target != volume_eeprom ){
        EEPROM.write(0, volume_target);
        volume_eeprom = volume_target;
      }
      int x = 3;
      while(x--){
        digitalWrite(BLINK_LED, HIGH);
        delay(100);
        digitalWrite(BLINK_LED, LOW);
        delay(100);
      }
  }

  if( vol_up != last_vol_up ){
    if( vol_up & volume_target < 31 ){
      volume_target++;
    }
    last_vol_up = vol_up;
  }

  if( vol_dn != last_vol_dn ){
    if( vol_dn & volume_target > 0 ){
      volume_target--;
    }
    last_vol_dn = vol_dn;
  }

  if(volume_target > volume_current){
    volume_up();
    changed = 0; // We don't need the extra delay if there's been a volume change delay
  }
  
  if(volume_target < volume_current){
    volume_down();
    changed = 0; // We don't need the extra delay if there's been a volume change delay
  }

  //if(changed)
  delay(20); // if an input changed we'll add a small delay to avoid "bouncing" of the input

}
