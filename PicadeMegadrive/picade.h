#include <arduino.h>

typedef struct
{
  int     player;
  char    key;      // keycode to emit when input is pressed
  int     pin;      // Arduino pin number for this input
  boolean select;
  boolean state;    // what state was the input last in ( HIGH/LOW )
} input;
