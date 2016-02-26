import serial
picade = serial.Serial('/dev/ttyACM0',9600,timeout=1)

'''Picade physical keys'''

JOY_UP    = 0
JOY_DOWN  = 1
JOY_LEFT  = 2
JOY_RIGHT = 3

BTN_1  = 4
BTN_2  = 5
BTN_3  = 6
BTN_4  = 7
BTN_5  = 8
BTN_6  = 9

BTN_START  = 10
BTN_COIN   = 11
BTN_ENTER  = 12
BTN_ESCAPE = 13

BTN_VOL_UP = 14
BTN_VOL_DN = 15

BTN_MOSI   = 16
BTN_MISO   = 17
BTN_SCLK   = 18

'''Picade actions/bindings'''

GAMEPAD_UP    = 220
GAMEPAD_DOWN  = 221
GAMEPAD_LEFT  = 222
GAMEPAD_RIGHT = 223
GAMEPAD_ZPOS  = 224
GAMEPAD_ZNEG  = 225

for x in range(16):
    globals()['GAMEPAD_' + str(x+1)] = 230 + x

ALT_1  = 252
ALT_2  = 253

VOL_UP = 250
VOL_DN = 251

KEY_LEFT_CTRL 	= 128
KEY_LEFT_SHIFT 	= 129
KEY_LEFT_ALT 	= 130
KEY_LEFT_GUI 	= 131
KEY_RIGHT_CTRL 	= 132
KEY_RIGHT_SHIFT = 133
KEY_RIGHT_ALT 	= 134
KEY_RIGHT_GUI 	= 135
KEY_UP_ARROW 	= 218
KEY_DOWN_ARROW 	= 217
KEY_LEFT_ARROW 	= 216
KEY_RIGHT_ARROW = 215
KEY_BACKSPACE 	= 178
KEY_TAB 	= 179
KEY_RETURN 	= 176
KEY_ESC 	= 177
KEY_INSERT 	= 209
KEY_DELETE 	= 212
KEY_PAGE_UP 	= 211
KEY_PAGE_DOWN 	= 214
KEY_HOME 	= 210
KEY_END 	= 213
KEY_CAPS_LOCK 	= 193
KEY_F1 		= 194
KEY_F2 		= 195
KEY_F3 		= 196
KEY_F4 		= 197
KEY_F5 		= 198
KEY_F6 		= 199
KEY_F7 		= 200
KEY_F8 		= 201
KEY_F9 		= 202
KEY_F10 	= 203
KEY_F11 	= 204
KEY_F12 	= 205

VALID_EVENTS    = range(13,255)

def read_response():
    response = ""
    while True:
        current = picade.readline()
        if current == "": break
        response += current
    
    return response.strip()

def command(string):
    try:
        picade.reset_input_buffer()
    except AttributeError:
        picade.flushInput()

    picade.write(string)

def dump():
    command('d')
    bindings = read_response()
    if bindings != '' and ' ' in bindings:
        return map(int,bindings.split(' ')[1:])
    return []

def keyboard():
    command('k')
    return 'Loaded Keyboard' in read_response()

def gamepad():
    command('g')
    return 'Loaded Gamepad' in read_response()

def key(key):
    return ord(key)

def save():
    command('s')

def load():
    command('l')

def bind(button, event=None, alt1=None, alt2=None):
    if type(button) is list:
        events = ' '.join(map(str, button))
        command("a {events}".format(events=events))
        return read_response()

    if button in range(19) and event in VALID_EVENTS:
        if alt1 is None or alt1 not in VALID_EVENTS:
            alt1 = ''

        if alt2 is None or alt2 not in VALID_EVENTS:
            alt2 = ''

        command("b {button} {event} {alt1} {alt2}".format(
            button=button,
            event =event,
            alt1  =alt1,
            alt2  =alt2
        ).strip())

    return read_response()
