#include <arduino.h>

/*
 * There are 32 discrete gain settings from +24dB maximum to -80dB minimum
 * On power up the gain is set to a default of 12dB
 */
#define VOL_DELAY  40

/* Buttons & Joystick */

#define BTN_1      A5  // PF0
#define BTN_2      A4  // PF1
#define BTN_3      A3  // PF4
#define BTN_4      A2  // PF5
#define BTN_5      A1  // PF6
#define BTN_6      A0  // PF7

#define LEFT       10  // PB6
#define RIGHT       9  // PB5
#define UP          6  // PD7
#define DOWN        8  // PB4

/* System Controls */

#define START      12  // PD6
#define COIN        4  // PD4      (1/4) on silk
#define ESCAPE      1  // PD3/TX
#define ENTER       0  // PD2/RX    1UP on silk

/* Audio & Amplifier */

#define VOL_UP      3  // PD0
#define VOL_DN      2  // PD1

#define HEADPHONE_DETECT 5  // PC6

#define AMP_UP      11 // PB7
#define AMP_DN       7 // PB6

#define BLINK_LED   13 // PC7

typedef struct
{
  char    key;      // keycode to emit when input is pressed
  char    pin;      // Arduino pin number for this input
  boolean state;    // what state was the input last in ( HIGH/LOW )
} input;

volatile int volume_current  = 0;
int volume_target   = 0;
int volume_saved    = 0;
int volume_eeprom   = 0;

boolean last_vol_up    = 0;
boolean last_vol_dn    = 0;
boolean last_headphone = 0;

void volume_target_save(){
  volume_saved = volume_target;
}

void volume_target_load(){
  volume_target = volume_saved;
  if( volume_target > 31 ) volume_target = 31;
}

void volume_up(){
  if(volume_current == 31) return;
  volume_current++;digitalWrite(AMP_UP, LOW);delay(VOL_DELAY);digitalWrite(AMP_UP, HIGH);delay(VOL_DELAY);
};

void volume_down(){ 
  if(volume_current == 0) return;
  volume_current--;digitalWrite(AMP_DN, LOW);delay(VOL_DELAY);digitalWrite(AMP_DN, HIGH);delay(VOL_DELAY);
};

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

/*
void volume_fade(int level){
  int transition_steps = abs(volume_current - level);
  int transition_delay = 0;
  if(transition_steps >= 1) transition_delay += VOL_DELAY;
  if(transition_steps >= 2) transition_delay += VOL_DELAY*10;
  if(transition_steps >  2) transition_delay += VOL_DELAY*4*(transition_steps-2);
  
  if(level < volume_current){
    digitalWrite(AMP_DN, LOW);
    delay(transition_delay);
    digitalWrite(AMP_DN, HIGH);
  }
 
  if(level > volume_current){
    digitalWrite(AMP_UP, LOW);
    delay(transition_delay);
    digitalWrite(AMP_UP, HIGH);
  }
  volume_current = level;
}*/

void volume_reset(){
  volume_current = 0;
  digitalWrite(AMP_DN, LOW);
  delay(140);  // Total time required for a complete drop from max volume: 1+10+(4*30)
  digitalWrite(AMP_DN, HIGH);
}
