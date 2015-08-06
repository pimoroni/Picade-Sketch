#Picade Updater

This is a BETA release of the Picade updater. It's been tested on a few units so far, but use with caution.

The supplied hex file will always be the latest build of the Picade software.

Don't use this method to update your Picade PCB with any other firmware unless you're equipepd to recover the bootloader!

For use with 2015 Picade boards only.

##Updating

( If your Picade is connected to your network, and you know how to use SSH, you can do that instead )

### Get to the shell
* Plug a keyboard into your Picade.
* Press your mapped "Start" button.
* Go to Quit
* Go to Quit Emulationstation

### Run the updater
If you've already cloned the updater, cd into the update folder and use `git pull` to update it. Otherwise:

* git clone https://github.com/pimoroni/Picade-Sketch
* cd into `picade-sketch/update`
* Run ./update
* avrdude will be automatically installed if it's missing.
* The updater should auto-reset your Picade PCB and continue
* If it doesn't, press Reset on your Picade and cross your fingers

##Changed in this build

* Picade buttons now have separate debounce times, they should be far more responsive
* Volume up/down buttons can be held down to smoothly adjust volume
* Volume should save to EEPROM after being adjusted, and restore at boot
* Bug fixes to volume control
