/*
  This file is a modified version of the original example here: https://github.com/MHeironimus/ArduinoJoystickLibrary
  
  Copyright (c) 2015, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef JOYSTICK_h
#define JOYSTICK_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

class Joystick_
{
private:
  int8_t   xAxis;
  int8_t   yAxis;
  int8_t   x2Axis;
  int8_t   y2Axis;
  uint16_t buttons;

public:
  Joystick_();

  void begin();
  void end();

  void setXAxis(int8_t value);
  void setYAxis(int8_t value);
  void setX2Axis(int8_t value);
  void setY2Axis(int8_t value);
  //void setZAxis(int8_t value);

  void setButton(uint8_t button, uint8_t value);

  void sendReport();
};
extern Joystick_ Joystick;

#endif
#endif
