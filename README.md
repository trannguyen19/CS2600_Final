# CS2600 FINAL(ESP#2)
 1. ESP32 will turn a LED on and off according to user input from publisher and show status "ON" and "OFF" 
 2. ESP32 will update the current temperature and humidity on the LCD screen and publish the these information throught mosquitto broker. The script will handle this message and print out whenever there are any changes in temperature and humidity. The script will also publish "HOT", "NORMAL", or "COLD" according to the temperature and the ESP32 will subscribe through mosquitto mqtt and intercept the LCD screen to show the message and change the color of the RGB LED accordingly.
 The ESP32 can also be controlled using and IR remote . It also has 2 more mode to either cycle throught the RGB and changing the speed, or set a specific RGB values to the RGB LED. We can always go back to the menu by pressing "Menu" or go back to the main automated function by pressing "Play".
To run the program:
```
.\script.sh
```
# Youtube Video

| Project Part| File        |
| ---|---|
| 1           |src\Final_1.ino|
| 2           |src\CS2600_Final.ino|



