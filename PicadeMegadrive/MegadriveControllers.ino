#include <keyboard.h> // Comment out if compile fails
#include "picade.h"

/*
These pin definitions map to the pinout
of the megadrive controller.

You will need to solder wires onto two 9-pin,
female d-sub connectors and connect them
to your Picade PCB.
*/

#define R_RIGHT A4
#define R_LEFT 10
#define R_DOWN A0
#define R_UP A1
#define R_B_A A2
#define R_C_START A3
#define R_SELECT 8

#define L_RIGHT 12
#define L_LEFT A5
#define L_DOWN 1
#define L_UP 0
#define L_B_A 2
#define L_C_START 3
#define L_SELECT 6

static int right_select = R_SELECT;
static int left_select =  L_SELECT;

static input inputs[] = {
  // Player 1
  { 0, KEY_UP_ARROW,   R_UP, HIGH },
  { 0, KEY_DOWN_ARROW, R_DOWN, HIGH },
  { 0, KEY_LEFT_ARROW, R_LEFT, HIGH },
  { 0, KEY_RIGHT_ARROW,R_RIGHT, HIGH },
  { 0, KEY_RETURN,     R_B_A, LOW },
  { 0, ' ',            R_B_A, HIGH },
  { 0, ' ',            R_C_START, HIGH },
  { 0, KEY_ESC,        R_C_START, LOW },
  // Player 2
  { 1, 'u', L_UP, HIGH },
  { 1, 'd', L_DOWN, HIGH },
  { 1, 'l', L_LEFT, HIGH },
  { 1, 'r', L_RIGHT, HIGH },
  { 1, 'f', L_B_A, LOW },
  { 1, 'g', L_B_A, HIGH },
  { 1, 'h', L_C_START, HIGH },
  { 1, 'j', L_C_START, LOW }
};

void setup(){
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++){
    pinMode(inputs[i].pin, INPUT_PULLUP);
  }
  
  pinMode(right_select, OUTPUT);
  pinMode(left_select,  OUTPUT);
  
  Keyboard.begin();
}

void process_inputs(int player, int select, boolean select_state){
  boolean changed = false;  // has any input changed?
 
  digitalWrite(select, select_state);
  delay(1);
  
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++){
    
    if(inputs[i].select != select_state) continue;
    if(inputs[i].player != player) continue;
    
    boolean state = !digitalRead(inputs[i].pin);
    
    if(state != inputs[i].state)
    {
      changed = true;
      inputs[i].state = state;
    }
    
    if(state){ Keyboard.press(inputs[i].key);}else{Keyboard.release(inputs[i].key);}
  } 
}

void loop(){
  process_inputs(0, right_select, LOW);
  process_inputs(0, right_select, HIGH);
  process_inputs(1, left_select, LOW);
  process_inputs(1, left_select, HIGH);
  delay(1);
}
