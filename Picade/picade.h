#include <arduino.h>
#include <EEPROM.h>

/*
 * There are 32 discrete gain settings from +24dB maximum to -80dB minimum
 * On power up the gain is set to a default of 12dB
 */
#define VOL_DELAY  40
#define VOL_MAX 24

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
  unsigned long last_change;
} input;

volatile int volume_current  = 0;
int volume_target   = VOL_MAX;
int volume_saved    = 0;
int volume_eeprom   = 0;

unsigned long last_vol_up_time = 0;
unsigned long last_vol_dn_time = 0;

boolean last_vol_up    = 0;
boolean last_vol_dn    = 0;
boolean last_headphone = 0;

void volume_target_save(){
  volume_saved = volume_target;
}

void volume_target_load(){
  volume_target = volume_saved;
  if( volume_target > VOL_MAX ) volume_target = VOL_MAX;
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

void volume_reset(){
  volume_current=34;
  while(volume_current > 0){
    volume_down();
  }
}

void load_volume_from_eeprom(){
  volume_reset();
  volume_target = EEPROM.read(0) & 0b00011111;
  volume_eeprom = volume_target;
}

