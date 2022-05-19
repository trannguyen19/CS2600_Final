/**********************************************************************
  Filename    : Temperature and Humidity Sensor
  Description : Use DHT11 to measure temperature and humidity.Print the result to the LCD1602.
  Auther      : www.freenove.com
  Modification: 2020/07/11
**********************************************************************/
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
const uint16_t recvPin = 15; // Infrared receiving pin
IRrecv irrecv(recvPin);      // Create a class object used to receive class
decode_results results;       // Create a decoding results class object

//remote

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

#define SDA 13                    //Define SDA pins
#define SCL 14                    //Define SCL pins

DHTesp dht;                       // create dht object
LiquidCrystal_I2C lcd(0x27,16,2); //initialize the LCD
int dhtPin = 18;                  // the number of the DHT11 sensor pin

//-RBG------------------

const byte ledPins[] = {15, 2, 4};    //define led pins
const byte chns[] = {0, 1, 2};        //define the pwm channels
//-RBG------------------

void setup() {
  Wire.begin(SDA, SCL);           // attach the IIC pin
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  dht.setup(dhtPin, DHTesp::DHT11); //attach the dht pin and initialize it


  //RBG
  for (int i = 0; i < 3; i++) {   //setup the pwm channels
    ledcSetup(chns[i], 1000, 8);
    ledcAttachPin(ledPins[i], chns[i]);
  }
  //RBG
  Serial.begin(115200);       // Initialize the serial port and set the baud rate to 115200
  irrecv.enableIRIn();        // Start the receiver
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(recvPin);   //print the infrared receiving pin
}

void loop() {

  
  //remote
  
  // read DHT11 data and save it 
  flag:TempAndHumidity DHT = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) {       //Determine if the read is successful, and if it fails, go back to flag and re-read the data
    goto flag;
  }  
  lcd.setCursor(0, 0);              //set the cursor to column 0, line 1
  lcd.print("Temperature:");        //display the Humidity on the LCD1602
  lcd.print(DHT.temperature);   
  lcd.setCursor(0, 1);              //set the cursor to column 0, line 0 
  lcd.print("Humidity   :");        //display the Humidity on the LCD1602
  lcd.print(DHT.humidity);  


  //RGB
   for (int i = 0; i < 256; i++) {
    setColor(wheel(i));
    delay(20);
  }

  //remote
  if (irrecv.decode(&results)) {          // Waiting for decoding
    serialPrintUint64(results.value, HEX);// Print out the decoded results
    Serial.println("");
    irrecv.resume();                      // Release the IRremote. Receive the next value
  }
  delay(10);
  
}



void setColor(long rgb) {
  ledcWrite(chns[0], 255 - (rgb >> 16) & 0xFF);
  ledcWrite(chns[1], 255 - (rgb >> 8) & 0xFF);
  ledcWrite(chns[2], 255 - (rgb >> 0) & 0xFF);
}

long wheel(int pos) {
  long WheelPos = pos % 0xff;
  if (WheelPos < 85) {
    return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
  } else {
    WheelPos -= 170;
    return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
  }
}
