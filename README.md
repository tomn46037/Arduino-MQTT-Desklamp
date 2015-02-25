
This is the firmware for my desklap.  I just moved into a cubicle
at the office and we like having the overhead lights off.  I would
like some task lighting though.

I decided to tie together some of the things I've been playing with
for a while and this is what I came up with.

I used an Arduino Uno, an Arduino Ethernet board and an Adafruit
Motor Driver board.  I also use Ponoko to laser cut some wood to
fit into the cubicle to mount the lights.

I can only use 3 of the motor channels on the motor driver because
channel 1 is taken up by SPI and the Ethernet port.  Channels 2, 3
and 4 are wired to some cheap 12v MR16 LED bulbs I bought off Ebay.

I am powering this from a 12V power supply.  I use the +5V from the
motor driver to run the Arduino.  (I'm not too concerned with noise
as I'm just driving LEDs insead of noisy motors.)

I'm also playing with MQTT lately.  I already have a broker setup
(yes, I've changed the password.  D'OH!  :)  The Arduino comes up
and requests a DHCP address then connects to the MQTT server and
sends a "hello" message.  I use the Arduino watchdog timer to reboot
if it fails to get a DHCP address or fails to connect to the MQTT
broker.

It then subscribes to "/arduino/lights/[1-4]".  Channel 1 is ignored
in the code as it would break the Ethernet if I tried to use it.
There is code to do a soft fade up/down to the desired level.  Once
the desired level is reached, it publishes a message back on
"/arduino/lights/status/[1-4]" with the current value.

Once I'm into the main loop, if the client should for some reason
become disconnected I enter a while(1) loops and let the watchdog
timer reboot.  This allows it to reconnect should anything happen.

My eventual goal is to write a JQuery page that will connect to
MQTT and allow me to adjust the lights.  When I get to that point,
I'd like to go ahead and post that code here as well.
