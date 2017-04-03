# Picade Updater

The supplied `picade.hex` hex file will always be the latest stable build of the Picade software.

Don't use this method to update your Picade PCB with any other firmware other than the ones we supply, unless you're equipepd to recover the bootloader!

*For use with 2015 Picade boards only. The original Kickstarter PCB has a different pinout.*

 ## Firmware Files

* other/picade_original.hex - Original Picade 2015 firmware
* other/picade_gamepad.hex - Gamepad-only firmware
* picade.hex - v2.3 LATEST reconfigurable Keyboard/Gamepad firmwarwe
* picade_v2.2.hex - v2.2 reconfigurable Keyboard/Gamepad firmware

## Updating

( If your Picade is connected to your network, and you know how to use SSH, you can do that instead )

### Get to the shell

* Plug a keyboard into your Picade.
* Press your mapped "Start" button.
* Go to Quit
* Go to Quit Emulationstation
* Answer yes, and make sure to press a button to prevent it relaunching

### Run the updater

If you've already cloned the updater, cd into the update folder and use `git pull` to update it. Otherwise:

* git clone https://github.com/pimoroni/Picade-Sketch
* cd into `picade-sketch/update`
* Run `./update`
* avrdude will be automatically installed if it's missing.
* The updater should auto-reset your Picade PCB and continue
* If it doesn't, press Reset on your Picade and cross your fingers

You can flash an alternate firmware by running `./update filename.hex`
