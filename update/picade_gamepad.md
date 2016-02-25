## Volume Control

picade_gamepad.hex has built-in, serial volume control.

You'll need to use minicom, Python or anything you fancy to talk Serial to the Picade PCB at 9600 baud.

Volume control is achieved with a very simple set of commands:

* `+`  - Raise volume 1 notch
* `-`  - Lower volume 1 notch
* `m`  - Mute
* `M`  - Set volume to max
* `fN` - Set volume to 'N' ( 0 to 24 ), ie: `f10`  or `f23`.
* `s`  - Save volume to EEPROM
* `v`  - Return current volume
* `l`  - Load volume from EEPROM

Headphones will also be detected and reported over serial, the volume muted and the current volume saved for restoring after headphone unplug. This will currently trounce any value you set while headphones are plugged in.

Once the source is published you can reconfigure to use button volume control, and assign VOL_UP/VOL_DN to any buttons you choose.

This serial fiddling is also a precursor to and practice for dynamic key binding on the keyboard version of Picade.

A reaallly simple example of how to do this in Python:

```python
import serial
picade = serial.Serial('/dev/ttyACM0',9600,timeout=0.1)
picade.write('+')
picade.readline().strip()
```

This increments the volume by one step, and reads back the response. You could couple this with your own controls connected to the Pi's GPIO to set the volume level, and maybe even a tiny LCD display to show the current volume. Hack away!
