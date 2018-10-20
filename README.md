# No-Battery-CPS2
Run CPS2 'B' boards with original roms without a battery

Forked from: https://github.com/ArcadeHacker/ArcadeHacker_CPS2

*** TESTED BY ME AND WORKING ON: ***
* 93646B-6 model B board (X-Men: Children of the Atom, xmcotau)
* 93646B-6 model B board (Marvel Super Heroes, mshu)
* 93646B-3 model B board (Alien vs Predator, avspu)

*** Current Issues reported by Artemio: ***
* ~~Not working on~~ B board revision 93646B-4 needs a 75 ohm resistor in series from the CLOCK signal (CN2 A31) to the Arduino (D11). He also stated he had to connect 5V from the B board to Vin on the Arduino rather than Arduino's 5V pin (this seems like this would cause too much voltage drop on the Arduino according to the specs, but maybe it's more tolerable than expected).
* B board revision 97691A-3 is very picky about 5v power (lower/higher slight voltage changes such as 4.85v/5.15v might make the Arduino unstable)

The purpose of this is to install a Arduino in each CPS B board so that it will write the decryption key every time you boot the game. This will allow you to keep the original unaltered roms, but be able to remove the battery and not have to worry about it leaking and causing damage to the PCB.

This is intended to be used on a Arduino Nano due to the small form factor, but should work on other Arduino versions as well. You will need to be connected (soldered) to the CPS2 B board as specified in the original ArcadeHacker_CPS2 programming guide, but without the need of a LCD Shield:

+5V     - Arudino 5V

DATA    - Arduino # 2

SETUP1  - Arduino # 3

CLOCK   - Arduino # 11

SETUP2  - Arduino # 12

GND     - Arduino GND

NOTE: You will need to edit the .ino file to change the value of "const unsigned char Key[] PROGMEM = ..." to the decryption key of the game you will be installing this in.
