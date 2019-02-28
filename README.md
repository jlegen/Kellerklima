# Kellerklima
## arduino based climate controller with indoor and outdoor sensors for fan control

I made this device to dry my (and others :) cellars by mostly making use of the times when the absolute humidity outdoors is lower than indoors. 
For this, two sensors are used to measure humidity and temperature, indoors and outdoors.
Though this seems to be quite a common design, i only found some rather costly commercial products. This one mostly uses cheap and popular modules.

## Basic principle
Temperature and humidity values from inside and outside are used to calculate the absolute humidity. If the outside is less humid than the inside, the fan is started (or, alternatively, the window is opened).
There's a maximum fan runtime to be configured. After this, either the fan is paused, or - using the 2nd relay - a dehumifier can be started for a period of time.
This runs until the target humidity value for the inside is reached.

## Implementation

Optionally, the data can also be sent to Thingspeak - making use of a tiny ESP8266 with ESPLink. 
This ESP-01 module also allows to remotely flash the arduino code of the controller. 
Libraries used are commented in the code.

Update: It turned out that the DHT22 sensors are cr*p, they "wear out" quite quickly, showing wrong humidity data after a few weeks.
To be able to make use of better quality I2C sensors like BME280 or SHT31, i added an I2C booster/bus driver IC to allow for the long(er) wires i needed. For me, 10m do work just fine with SHT31, there are reports that up to 100m are possible, too.
Additionally, i did not manage to get the SLIP protocol running reliably using the ESP-Link, but i still regard this as useful, since the arduino not only can be flashed "OTA" using ESP-link, the new version also allows to monitor the arduino's activities using EPS-link's "µC Console", and the whole thing can be controlled remotely using a set of commands to set temperatures and other parameters. 
Next hardware revision should probably use an ESP module as controller... 


# Gallery

box in normal run mode; the LCD is switched off after a few minutes of inactivity

![1st "production" version](pics/box_1.png)

rotary encoder at the right - is used to scroll through values, testing the relay, and to set most parameters

![1st "production" version](pics/box_2.png)

PCB version finally ready :-)
![1st PCB version](pics/pcb_1.jpg)

new PCB for sensors - tailored to fit into cheap weather-proof boxes
![Sensor pcp 1.2](pics/Sensors_pcb_V1.2.jpg)
