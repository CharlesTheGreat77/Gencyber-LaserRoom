# Gencyber-LaserRoom 🖲️
This is the code for the laser room (Escape Room) for the GenCyber summer camp. 

# Hardware 🗜️
| Hardware                       |
|--------------------------------|
| ESP8266 NodeMCU v3   ±$5-10    |
|--------------------------------|
| Solar Post Cap       ±$5-10    |
|--------------------------------|
| Lasers               ±$5-10    |
|--------------------------------|
| LED Strip (optional) ±$10      |
|--------------------------------|
| Mirrors (optional)   ±$5       |
|--------------------------------|


<details>
  <summary>Images/Videos</summary>

  ## Test Run (Without Mirrors)
  ![TestRUN](https://github.com/CharlesTheGreat77/Gencyber-LaserRoom/assets/27988707/44d92907-4415-4edd-910d-ad1e5359191b)

  ## Laser and Sensor(s)
  ![LaserSensor](https://github.com/CharlesTheGreat77/Gencyber-LaserRoom/assets/27988707/703044cb-060e-4b8a-bc64-a07f98c1bcb5)

  ## Relay(s)
  ![Relay](https://github.com/CharlesTheGreat77/Gencyber-LaserRoom/assets/27988707/2648bcdc-ac58-4274-9618-2df230cf8dfc)

# How it works ⚙️
The lasers are connected to a single ESP8266 NodeMCU V3 on the pins specifed in the ```lasercontrol.ino``` file. All ESPs are connected to a single AP, which allows seemless control of the LEDs and the lasers. The laser is pointed directly at the outdoor solarpost cap "sensors" that are wired to an esp8266. LED strips are wired to a single ESP8266 with an external power supply to provide 5v to the LEDs. The sensor(s) on boot calibrates with the laser sensor values being the "norm". Any huge drop in the values will signify that someone/something got in the way of the laser and sensor. The sensors send an http request to the laser controlled ESP8266 which then turn OFF/ON the lasers and turns the LEDs red when tripped for the seconds specified in the ```LaserSensor.ino``` file. 

# Arduino Dependencies 📝
| Dependencies                   |
|--------------------------------|
| ESP8266 Generic Arduino Lib    |
|--------------------------------|
| FastLED Arduino Lib            |
|--------------------------------|
Board Manager URL for ESP8266 Generic Lib:
- https://arduino.esp8266.com/stable/package_esp8266com_index.json


# Setup 🔨
1. Specify the AP and Password the ESP(s) will be on in all of the INO files.
2. Change the IP addresses in ```LaserSensor.ino``` file to the IP Addresses of the laser ESP, and the LED ESP.
3. Setup laser(s) and mirror positions.
4. Setup the LEDs. Change number of LEDs in the ```EAPLedLights.ino``` file.
5. Make sure the lasers are hitting the solarpost cap sensors DIRECTLY.
6. Power on the sensors with the lasers directly hitting the sensor. (takes only a few seconds after boot to calibrate)

# Control 🕹️
There's two options to control the lasers and LEDs.
1. Terminal
   - curl http://192.168.0.101/laser/off
   - curl http://192.168.0.101/laser/on?num=5 // [off/on] turn laser 5 on (each laser is controlled via GPIO Pins on ESP8266)
   - curl http://192.168.0.102/led/off // [off/on] turn off LEDs
   - curl http://192.168.0.102/led/on?color=white // [red/white] only two options for colors, change in ```EAPLedLights.ino```
2. Web Interface
   - Compile and run the ```web_interface.go``` file to start the local webserver to control the lasers with the ```index.html``` in the repo.
   - ```go build -o web_interface web_interface.go``` // to compile
   - ```./web_interface``` // run the webserver

# Conlusion
With all the work done, I am glad the students enjoyed the laser room, and glad all worked as expected. We had issues with the power distribution with the lasers, but after rigging a 9v to each, we seemed to get through the hours of it operating.

More Pictures/Videos SOON!
