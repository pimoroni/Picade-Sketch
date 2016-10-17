# Picade 2015 Firmware

This is the firmware sourcecode for the retail Picade PCB released in 2015.

# Using Picade

The Picade PCB includes a configuration interface over USB serial, it uses a series of commands you can type by hand or automate with Python.

You can bind any input on the Picade PCB to a variety of actions, from keyboard keys to gamepad buttons/directions.

Picade also supports two shifted states- you can assign up to two buttons to act as 'shift' internally, giving you up to two alternate functions for each input. These are referred to as `alt1` and `alt2` below.

## Available Commands

### Help:

Send just an `h` to your Picade PCB for a complete built-in help guide.

### Bind a single key:

```
b <button> <keycode> <optional:alt1> <optional:alt2>
```

Binds a single button on the Picade to a keyboard key or action. The `alt1` and `alt2` are optional, and assign actions to the different shift states.

See below for available choices for buttons and keycodes.

### Bind all keys:

```
a <keycode> <keycode> ...
```

Usually used to restore a dumped set of keycodes back to the Picade.

See the dump command for more information on what data this command supports.

### Dump all keys:

To get a dump of all keys bound, send `d`.

This will return a long list of 59 numbers:

* 19 mapping codes for normal key actions
* 19 mapping codes for alt1 key actions
* 19 mapping codes for alt2 key actions
* 1 byte of shift state information
* the saved volume

Each block of 19 mapping codes contains an action for each picade button, in the following order:

```
UP, DOWN, LEFT, RIGHT, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, START, COIN, ENTER, ESCAPE, VOL_UP, VOL_DN, MOSI, MISO, SCLK
```

### Save & Load:

To save, send 's' and to load send 'l'. Your mappings are saved to EEPROM on the Picade PCB and loaded automatically at startup. To save multiple profiles for switching later, you should use dump and bind documented above. 

### Quickbinds:

Picade includes a few built-in profiles to get you started:

* Quick keyboard, send `k` - All Picade buttons and joystick directions send keyboard codes
* Quick gamepad, send `g` - Interprets the U/D/L/R gamepad joystick directions from the digital stick
* Quick dual analog, send `n` - If you've connected two analog joysticks to your Picade, this will give you a dual analog controller setup

## Picade Buttons

Each of these numbers denotes a button input on the Picade PCB. You can optionally bind the connections on the SPI header (the group of 6 pins).

```
0  = Joy U    1  = Joy D    2  = Joy L    3  = Joy R    
4  = Btn 1    5  = Btn 2    6  = Btn 3    7  = Btn 4    8  = Btn 5    9  = Btn 6    
10 = Start    11 = Coin     12 = Enter    13 = Escape   
14 = Vol Up   15 = Vol Down 
16 = MOSI     17 = MISO     18 = SCLK     
```

### SPI Header

The SPI header is the group of 6 pins next to the reset button. With the Picade text the right way up, the VCC pin is the top left most, and Ground the top right most pin.

```text
  [ RESET  BUTTON ]
                      1 = MISO  3 = SCK
  [ 2 ] [ 4 ] [ 6 ]   2 = VCC   5 = RESET
  [ 1 ] [ 3 ] [ 5 ]   3 = SCK   6 = GND 
```

## Picade Mapping Codes

The following is a complete table of keycodes, gamepad codes and actions which can be mapped to a Picade button.

```
220 = Gpad U   221 = Gpad D   222 = Gpad L   223 = Gpad R   224 = Gpad Z+  225 = Gpad Z-  
246 = Joy 1 X  247 = Joy 1 Y  248 = Joy 2 X  249 = Joy 2 Y  
230 = Gpad 0   231 = Gpad 1   232 = Gpad 2   233 = Gpad 3   
234 = Gpad 4   235 = Gpad 5   236 = Gpad 6   237 = Gpad 7   
238 = Gpad 8   239 = Gpad 9   240 = Gpad 10  241 = Gpad 11  
242 = Gpad 12  243 = Gpad 13  244 = Gpad 14  245 = Gpad 15  

250 = Vol +   251 = Vol -   252 = Alt 1   253 = Alt 2   
218 = U Arrow 217 = D Arrow 216 = L Arrow 215 = R Arrow 
128 = L Ctrl  129 = L Shift 130 = L Alt   131 = L GUI   
132 = R Ctrl  133 = R Shift 134 = R Alt   135 = R GUI   
178 = Backspc 179 = TAB     176 = Return  177 = ESC     
209 = Insert  212 = Delete  211 = Pg Up   214 = Pg Down 
210 = Home    213 = End     193 = Caps Lk 13  = Space   
194 = F1      195 = F2      196 = F3      197 = F4      
198 = F5      199 = F6      200 = F7      201 = F8      
202 = F9      203 = F10     204 = F11     205 = F12     

33  = !       34  = "       35  = #       36  = $       
37  = %       38  = &       39  = '       40  = (       
41  = )       42  = *       43  = +       44  = ,       
45  = -       46  = .       47  = /       48  = 0       
49  = 1       50  = 2       51  = 3       52  = 4       
53  = 5       54  = 6       55  = 7       56  = 8       
57  = 9       58  = :       59  = ;       60  = <       
61  = =       62  = >       63  = ?       64  = @       
65  = A       66  = B       67  = C       68  = D       
69  = E       70  = F       71  = G       72  = H       
73  = I       74  = J       75  = K       76  = L       
77  = M       78  = N       79  = O       80  = P       
81  = Q       82  = R       83  = S       84  = T       
85  = U       86  = V       87  = W       88  = X       
89  = Y       90  = Z       91  = [       92  = \       
93  = ]       94  = ^       95  = _       96  = `       
97  = a       98  = b       99  = c       100 = d       
101 = e       102 = f       103 = g       104 = h       
105 = i       106 = j       107 = k       108 = l       
109 = m       110 = n       111 = o       112 = p       
113 = q       114 = r       115 = s       116 = t       
117 = u       118 = v       119 = w       120 = x       
121 = y       122 = z       123 = {       124 = |       
125 = }       126 = ~       
```