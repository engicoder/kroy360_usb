Kroy 360 USB converter with display
======================

This converter interfaces the display and keyboard directly, bypassing the 
original main board. The controller receives key events from the keyboard 
and maps them into USB key codes which are then sent via the USB connection to 
the host. The key events are simultaneously translated into ascii characters 
to be output on the lcd display. Text from the keyboard is displayed as a single 
line which is cleared when pressing enter or escape. The key stroke count of the
current line is also displayed.

## Interfacing

The converter PCB connects to the display using the 10 pin header marked 
"DISPLAY" and to the keyboard via a 20 pin ribbon cable connected to the header
marked "KEYBOARD". The other end of the ribbon cable connects to the 20 pin 
header on the keyboard assembly. 

###  Pinouts

#### Keyboard
<table>
    <tr><td>1</td><td>P1 (pulse 1)</td><td>2</td><td>P2 (pulse 2)</td></tr>
    <tr><td>3</td><td>KS (key state)</td><td>4</td><td>EM_LED</td></tr>
    <tr><td>5</td><td>D6 (data 6)</td><td>6</td><td>TS_LED</td></tr>
    <tr><td>7</td><td>D5 (data 5)</td><td>8</td><td>CL_LED</td></tr>
    <tr><td>9</td><td>D4 (data 4)</td><td>10</td><td>GND</td></tr>
    <tr><td>11</td><td>D3 (data 3))</td><td>12</td><td>GND</td></tr>
    <tr><td>13</td><td>D2 (data 2)</td><td>14</td><td>GND</td></tr>
    <tr><td>15</td><td>D1 (data 1)</td><td>16</td><td>GND</td></tr>
    <tr><td>17</td><td>D0 (data 0)</td><td>18</td><td>GND</td></tr>
    <tr><td>19</td><td>VCC (KB pwr)</td><td>20</td><td>EX</td></tr>
</table>   

#### Display
<table>
    <tr><td>1</td><td>FLM</td></tr>
    <tr><td>2</td><td>M</td></tr>
    <tr><td>3</td><td>CL1</td></tr>
    <tr><td>4</td><td>D</td></tr>
    <tr><td>5</td><td>CL2</td></tr>
    <tr><td>6</td><td>VDD (+5V)</td></tr>
    <tr><td>7</td><td>VSS</td></tr>
    <tr><td>8</td><td>VEE (-5V)</td></tr>
    <tr><td>9</td><td>VO (bias)</td></tr>
    <tr><td>10</td><td>NC</td></tr>
</table>   

### Operation

#### Data
Data is received from the keyboard as a 7 bit scan code on data lines *D0..D6*. 
Each key position has a unique scan code. The up or down state of the key is set
by the *KS* line with a high value indicating the down state and a low value 
indicating the up state.  The *P1* line is pulled low by the keyboard when a key
state change has been detected. The converter reads the data including key state
and then pulls *P2* low momentary (~20us) to release the keyboards data latch.
If multiple keys are down, the *KS* line indicates the state of the last key change.

#### Status LEDs
The keyboard has 3 keys with integrated status LEDs; *Caps Lock*, *Type Specs* and
*Edit Mode*. Each LED has a dedicated interface line. Pulling the line low turns 
the LED on.

#### Display
The display is a Hitachi H2525 239x20 dot graphical lcd display. The display 
does not have a dedicated controller. The rows and columns of the display are 
driven by a chain of HD44104 shift registers. Data must be sent as a continuous,
synchronous data stream using data line *D* and clock line *CL2*. The *CL1* line 
latches each 239 bit row of data. The *FLM* line indicates the beginning of a new 
frame of data. Each frame is 20 rows of data. The *M* line is the polarity of the 
LCD drive voltage. The *M* line should be toggled every frame or the intensity of 
the pixels will fade. The display requires both a +5V (*VDD*) and -5V (*VEE*) 
supply. The *VO* line a bias voltage that changes the angle from which the pixels 
can be seen.

## Hardware

The custom PCB uses an ATmega32U4 microcontroller to read data from the keyboard,
drive the display, and send received key events over USB to the host by means of
a custom firmware driver. The +5V and -5V supplies for the display are generated
from the 5V provided by USB using the TC1044S charge pump. The display bias 
voltage VO is created using a potentiometer. The outer pins of the potentiometer
are attached to +5V and -5V; the center tap connected to VO. A TPS22929D load 
switch allows the firmware to control when USB 5V power is connected to the 
keyboard VCC pin. An on board LED is used for firmware status. It will flash 
during start up and once when a key event is received. 

## Firmware

The firmware is written to use the QMK keyboard firmware framework. 
https://github.com/qmk/qmk_firmware
### Build
To build the firmware:

* Clone the QMK repository and sub modules into a local folder
* Create a new folder, 'kroy360_usb', under 'qmk_firmware/keybords/converter'
* Copy the contents of the 'firmware' directory in this repository to the new 
'kroy360_usb' folder

From the qmk_firmware root:

    $ make converter/kroy360_usb:default

### Features

#### Password blanking
A custom QMK keycode, **PWD_TOG**, has been defined that can be assigned to any key.
When pressed, this keycode will toggle the password blanking feature. When active 
password blanking replaces the ascii characters for a given keystroke with an 
asterisk '*'. In the default keymap, **PWD_TOG** is assigned to the *Type Specs* 
key and the integrated LED will turn on when password blanking is active.