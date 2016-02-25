#Picade Config

##Python

picade.py is a Python library for rebinding Picade controls on the fly.

Use it like so:


### Single Button

Tell Picade to bind the joystick up to gamepad up:

```python
import picade

picade.bind(picade.JOY_UP, picade.GAMEPAD_UP)
```

### Multiple Buttons

Tell Picade to send joystick up/down/left/right to the gamepad:


```python
import picade

picade.bind([picade.GAMEPAD_UP, picade.GAMEPAD_DOWN, picade.GAMEPAD_LEFT, picade.GAMEPAD_RIGHT])
```

Or back to the keyboard:

```python
import picade

picade.bind([picade.KEY_UP_ARROW, picade.KEY_DOWN_ARROW, picade.KEY_LEFT_ARROW, picade.KEY_RIGHT_ARROW])
```
