#Picade Megadrive Firmware

This firmware is intended for advanced users. You'll need:

* Some original 3 button SEGA Megadrive controllers
* Two 9-pin, female D-Sub connections
* Single core breadboard wire or similar
* Somewhere to solder

This is the Megadrive controller pinout
as seen from the front and back of a 
9-pin d-sub connector.
  
```text
    --- FRONT ---   ---- BACK ----
  
      1 2 3 4 5       5 4 3 2 1
    _____________   _____________
    \ 0 0 0 0 0 /   \ 0 0 0 0 0 /
     \_0_0_0_0_/     \_0_0_0_0_/
   
       6 7 8 9         9 8 7 6
```

Use the right/back view when soldering,
if you hold a 9-pin d-sub up and look at it
from the side you solder to, this will be the
pinout. It probably has numbers written on it anyway!
     
    Pin | Select == LOW | Select == HIGH
    1   | Up            | Up
    2   | Down          | Down
    3   | -             | Left
    4   | -             | Right
    5   | +5VDC         | +5VDC
    6   | Button A      | Button B
    7   | Select        | Select
    8   | Ground        | Ground
    9   | Start         | Button C
   
Megadrive controller pinout: http://pinouts.ru/Game/genesiscontroller_pinout.shtml
