#Picade Updater

This is a BETA release of the Picade updater. It's been tested on a few units so far, but use with caution.

The supplied hex file will always be the latest build of the Picade software.

Don't use this method to update your Picade PCB with any other firmware unless you're equipepd to recover the bootloader!

For use with 2015 Picade boards only.

##Updating

* Run ./update
* avrdude will be automatically installed if it's missing.
* The updater should auto-reset your Picade PCB and continue
* If it doesn't, press Reset on your Picade and cross your fingers

##Changed in this build

* Picade buttons now have separate debounce times, they should be far more responsive
* Volume up/down buttons can be held down to smoothly adjust volume
* Volume should save to EEPROM after being adjusted, and restore at boot
* Bug fixes to volume control
